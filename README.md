# gesture-daemon
A userland daemon for handling sleep mode gestures in Sailfish OS.

## Configuration
Enabled gestures are controlled via dconf @ `/apps/onyxgestures/enabled-gestures`:
```
[apps/onyxgestures]
enabled-gestures=['double_tap', 'flashlight', 'music', 'camera', 'voicecall']
```

## Adaptation
If your device supports this feature, getting sleep mode gestures working in SFOS may need the following changes:

1. [Make touchscreen driver emit EV_MSC and MSC_GESTURE events instead of EV_KEY and KEY_* ones](https://github.com/sailfishos-oneplus5/android_kernel_oneplus_msm8998/commit/93690764d52358007b2312e48fd9b38040577f9f)
2. [Create an MCE config to match the touchscreen driver emitted codes](https://github.com/sailfishos-oneplus5/droid-config-cheeseburger/blob/hybris-16.0/sparse/etc/mce/90-display-gestures.conf)
3. Fork and edit the sources as needed to match your device's `/proc/touchpanel/*` entries
	1. If you have a different `${gesture}_enable` entry for different gestures => change the existing sources as you wish
	2. If you have a single `gesture_enable` entry => [revert the following commit](https://github.com/sailfishos-oneplus5/gesture-daemon/commit/03a4a88f15acdca75a3321d69e27fce09e01705d)
