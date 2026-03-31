### v1.10
- Decoupled from Screen Space Shaders addon.
### v1.9
- Improved Bokeh DoF quality.
    - Added denoising pass to DoF so the final image will be way less noisy.
    - Due to this improvement, DoF's default quality setting has been lowered to INSPECT_DOF_QUALITY_MEDIUM.
    This should provide better performance while maintaining the rendering quality.
- Moved Bokeh DoF shader into its own patch in FOMOD.
### v1.8-hotfix
- Fixed crash caused by invocations of undefined functions.
### v1.8
- Added "Disable Weapon Idle Animation" option in MCM.
    - When enabled, this option disables weapon idle animations from playing automatically by game so it won't interrupt inspection.
    This option won't take effect with Anomaly 1.5.2 or earlier.
### v1.7
- Improved compatibility with [Beef's Shader Based NVGs](https://www.moddb.com/mods/stalker-anomaly/addons/beefs-shader-based-nvgs-v10) and ["Better" Beef's NVGs](https://www.moddb.com/mods/stalker-anomaly/addons/better-beefs-nvg-indiesunpraiser) addons.
    - Game now will use NVGs' DoF instead when user inspects weapon with NVG on.
        Since NVGs use fixed focal point, it doesn't make any sense that users are able to focus on their weapon in the foreground.
### v1.6
- Improved Bokeh DoF quality.
    - Apply tonemap to sampled color so when it receives highlight boost, the color will be more natural and avoids white washout.
    - Due to this improvement, INSPECT_DOF_HIGHLIGHT_THRESHOLD setting has been removed due to redundancy.
### v1.5
- Implemented Circle of Confusion based Bokeh DoF for r3 and r4 renderers.
Game now will render high quality Bokeh DoF during inspection instead of the original gaussian blur based DoF.
    - The parameters of DoF can be configured in <u>gamedata\shaders\r3\inspectWeapon_dof_settings.h</u>
        - Configurable parameters:
            - **INSPECT_DOF_QUALITY_***: Controls how many samples should be gathered when calculating DoF.
            - **INSPECT_DOF_RADIUS**: Controls the blur radius of DoF.
            - **INSPECT_DOF_HIGHLIGHT_THRESHOLD**: Controls the threshold of pixel luminance that receives highlight boost.
            - **INSPECT_DOF_HIGHLIGHT_GAIN**: Controls the amount of pixel's highlight boost.
### v1.4
- Improved handling of inspection interruption due to weapon state changes.
- General stability improvements.
### v1.3
- Added "Double tap" key input mode in MCM.
- Fixed an issue when "Remove UI During Weapon Inspection" option is enabled, if user quits game, starts a new game or loads a save during weapon inspection, in-game UI and crosshair will be disabled regardless of user's setting.
    - This fix does not account for the case where user presses Alt + F4 to force game to terminate.
### v1.2
- Added support for ammo check, now it has **33%** chance to trigger everytime user inspects their weapon.
- Improved handling for weapons with grenade launcher that have various inspect animations.
- Fixed a potential issue where spamming inspection might cause corrupted animation and UI state.
- Renamed addon name in MCM.
- Small code optimizations.
### v1.1
- Fixed an issue when "Remove UI During Weapon Inspection" option is enabled, user's in-game UI and crosshair setting resets after inspection animation completes.
- Renamed some options in MCM.
- Small code optimizations.
### v1.0
- Initial release.
