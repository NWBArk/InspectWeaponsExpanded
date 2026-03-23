# Inspect Weapons Expanded
## An expanded version of Inspect Weapons addon for STALKER Anomaly.

Original addon: [Inspect Weapon, and Inspect weapon for FireModeCheck - fsttime](https://www.moddb.com/mods/stalker-anomaly/addons/mcminspect-weapon).

This expanded version aims to recreate the weapon inspection feature presented in modern FPS games, such as Call of Duty.

MCM and FOMOD are supported.

By default, you can <u>**long press "R"**</u> to trigger weapon inspection.

Depending on the weapon packs you use, <u>some weapons might not provide a suitable animation thus they can't be inspected</u>. This is not a bug of this addon.

### New features:
- **Customizable key input mode**: Choose between single press, double tap or long hold to trigger the inspection.
- **High quality Depth of Field effect**: Choose whether to enable high quality Bokeh DoF during the animation or not.
- **Remove UI**: Choose whether to remove in-game UI during the animation or not.

### Changelog:
See [Changelogs](/CHANGELOG.md).

### Current limits:
- Requires Screen Space Shaders as this addon needs to call SSS' weapon DoF command.
- DOF's fade start/length are not customizable at the moment.
- In-game UI doesn't have fade in/out animation when inspection starts/ends.

### Special Thanks
- [fsttime](https://www.moddb.com/members/fsttime) - Author of the original addon
- [artifax](https://www.moddb.com/members/artifax) - Original weapon inspection logics
- [Ascii1457](https://www.moddb.com/members/ascii1457) - SSS and DoF scripting logics
- [IndieSunpraiser](https://www.moddb.com/members/indiesunpraiser) - NVG DoF shader code