#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <net/cfg80211.h>


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

struct wi_cfg80211 {
	struct wireless_dev *wdev;
};

static struct cfg80211_ops my_cfg80211_ops = {
};


struct wireless_dev *wdev;

static int __init hello_init(void)
{
	int ret = 0;
	struct wiphy *wiphy;

	wdev = kzalloc(sizeof(struct wireless_dev), GFP_KERNEL);
	if (!wdev) {
		pr_err("wdev memory allocation failed\n");
		return -ENOMEM;
	}

	wiphy = wiphy_new(&my_cfg80211_ops, sizeof(struct wi_cfg80211));
	if (!wiphy) {
		pr_err("wiphy_new() failed\n");
		return -ENOMEM;
	}

	wdev->wiphy = wiphy;

	wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION);
	wiphy->bands[NL80211_BAND_2GHZ] = &band_2ghz;

	ret = wiphy_register(wiphy);
	if (ret < 0) {
		pr_err("wiphy_register() failed, ret=%d\n", ret);
	}

	return ret;
}

static void __exit hello_exit(void)
{
	if (!wdev) 
		return;

	if (!wdev->wiphy)
		goto free_wdev;	

	wiphy_unregister(wdev->wiphy);
free_wdev:
	kfree(wdev);
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Neo Jou <neojou@gmail.com>");
MODULE_LICENSE("GPL");
