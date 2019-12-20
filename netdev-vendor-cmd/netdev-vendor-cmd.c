#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <net/cfg80211.h>
#include <net/netlink.h>

#define WIPHY_NAME "wi"
#define NDEV_NAME "wi%d"


#define CHAN2G(_channel, _freq, _flags) { 		\
	.band			= NL80211_BAND_2GHZ, 	\
	.center_freq		= (_freq),		\
	.hw_value		= (_channel),		\
	.flags			= (_flags),		\
	.max_antenna_gain	= 0,			\
	.max_power		= 30,			\
}

#define RATETAB_ENT(_rate, _rateid, _flags) 		\
	{						\
		.bitrate = (_rate),			\
		.hw_value = (_rateid),			\
		.flags = (_flags),			\
	}

static struct ieee80211_channel _2ghz_channels[] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
};

static struct ieee80211_rate _2ghz_rates[] = {
	RATETAB_ENT(10, 0x1, 0),
	RATETAB_ENT(20, 0x2, 0),
	RATETAB_ENT(55, 0x4, 0),
	RATETAB_ENT(110, 0x8, 0),
	RATETAB_ENT(60, 0x10, 0),
	RATETAB_ENT(90, 0x20, 0),
	RATETAB_ENT(120, 0x40, 0),
	RATETAB_ENT(180, 0x80, 0),
	RATETAB_ENT(240, 0x100, 0),
	RATETAB_ENT(360, 0x200, 0),
	RATETAB_ENT(480, 0x400, 0),
	RATETAB_ENT(540, 0x800, 0),
};

#define g_rates	(_2ghz_rates + 0)

static struct ieee80211_supported_band band_2ghz = {
	.band = NL80211_BAND_2GHZ,
	.channels = _2ghz_channels,
	.n_channels = ARRAY_SIZE(_2ghz_channels),
	.bitrates = g_rates,
	.n_bitrates = 12,
};

struct wi_context {
	struct wiphy *wiphy;
	struct net_device *ndev;
};

struct wi_wiphy_priv {
	struct wi_context *navi;
};

struct wi_netdev_priv {
	struct wi_context *navi;
	struct wireless_dev wdev;
};

static struct cfg80211_ops my_cfg80211_ops = {
};


#define OUI_GOOGLE	0x001A11
#define OUI_REALTEK	0x00E04C

enum wi_vendor_subcmd {
	VENDOR_SUBCMD_ADD,
};

static int vendor_do_cmd_add(struct wiphy *wiphy, struct wireless_dev *wdev,
			const void *data, int len);

static const struct wiphy_vendor_command wi_vendor_cmds[] = {
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = VENDOR_SUBCMD_ADD,
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.policy = VENDOR_CMD_RAW_DATA,
		.doit = vendor_do_cmd_add
	},
};

static struct wi_wiphy_priv *
wiphy_get_navi_context(struct wiphy *wiphy)
{
	return (struct wi_wiphy_priv *)wiphy_priv(wiphy);
}

static struct wi_netdev_priv *
ndev_get_navi_context(struct net_device *ndev)
{
	return (struct wi_netdev_priv *)netdev_priv(ndev);
}

static int wi_tx(struct sk_buff *skb, struct net_device *dev)
{
	pr_info("%s enter\n", __func__);

	/* Dont forget to cleanup skb, as its ownership moved to xmit callback */
	kfree_skb(skb);
	return NETDEV_TX_OK;
}

static const struct net_device_ops wi_netdev_ops = {
	.ndo_start_xmit	= wi_tx,
};



static int vendor_do_cmd_add(struct wiphy *wiphy, struct wireless_dev *wdev,
			const void *data, int len)
{
	int i, sum;
	const u8 *ptr = data;
	struct sk_buff *skb;


	pr_info("%s enter, len=%d\n", __func__, len);

	sum = 0;
	for (i=0; i<len; i++) {
		pr_info("%s: data=%d\n", __func__, ptr[i]);
		sum += ptr[i];
	}

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, sizeof(int));
	if (unlikely(!skb)) {
		pr_err("skb alloc failed\n");
		return -ENOMEM;
	}

	nla_put_nohdr(skb, sizeof(int), &sum);

	return cfg80211_vendor_cmd_reply(skb);
}

static struct wi_context *wi_create_context(void)
{
	struct wi_context *ret = NULL;
	struct wi_wiphy_priv *wiphy_data = NULL;
	struct wi_netdev_priv *ndev_data = NULL;
	unsigned long kflags;


	kflags = in_atomic() ? GFP_ATOMIC : GFP_KERNEL;
	
	ret = kzalloc(sizeof(*ret), kflags);
	if (!ret) {
		pr_err("wi_context memory allocation failed\n");
		goto l_error;
	}

	ret->wiphy = wiphy_new_nm(&my_cfg80211_ops,
			sizeof(struct wi_wiphy_priv), WIPHY_NAME);
	if (ret->wiphy == NULL) {
		goto l_error_wiphy;
	}

	wiphy_data = wiphy_get_navi_context(ret->wiphy);
	wiphy_data->navi = ret;

	ret->wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION);
	ret->wiphy->bands[NL80211_BAND_2GHZ] = &band_2ghz;

	ret->wiphy->vendor_commands = wi_vendor_cmds;
	ret->wiphy->n_vendor_commands = ARRAY_SIZE(wi_vendor_cmds);

	if (wiphy_register(ret->wiphy) < 0) {
		pr_err("wiphy_register() failed\n");
		goto l_error_wiphy_register;
	}
		
	ret->ndev = alloc_netdev(sizeof(*ndev_data), NDEV_NAME,
				NET_NAME_ENUM, ether_setup);
	if (ret->ndev == NULL) {
		pr_err("alloc_netdev() failed\n");
		goto l_error_alloc_netdev;
	}

	ndev_data = ndev_get_navi_context(ret->ndev);
	ndev_data->navi = ret;

	ndev_data->wdev.wiphy = ret->wiphy;
	ndev_data->wdev.netdev = ret->ndev;
	ndev_data->wdev.iftype = NL80211_IFTYPE_STATION;
	ret->ndev->ieee80211_ptr = &ndev_data->wdev;

	ret->ndev->netdev_ops = &wi_netdev_ops;

	if (register_netdev(ret->ndev)) {
		pr_err("register_netdevice failed\n");
		goto l_error_netdev_register;		
	}

	return ret;

l_error_netdev_register:
	free_netdev(ret->ndev);	

l_error_alloc_netdev:
	wiphy_unregister(ret->wiphy);

l_error_wiphy_register:
	wiphy_free(ret->wiphy);

l_error_wiphy:
	kfree(ret);
	
l_error:
	return NULL;
}

static void wi_free(struct wi_context *ctx)
{
	if (ctx == NULL)
		return;

	unregister_netdev(ctx->ndev);
	free_netdev(ctx->ndev);
	wiphy_unregister(ctx->wiphy);
	wiphy_free(ctx->wiphy);
	kfree(ctx);
}

static struct wi_context *g_ctx = NULL;

static int __init hello_init(void)
{
	g_ctx = wi_create_context();

	return g_ctx == NULL;
}

static void __exit hello_exit(void)
{
	wi_free(g_ctx);
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");
