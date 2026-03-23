/*
    Controls DoF quality. Available value:
        INSPECT_DOF_QUALITY_LOW
        INSPECT_DOF_QUALITY_MEDIUM
        INSPECT_DOF_QUALITY_HIGH
        INSPECT_DOF_QUALITY_VERYHIGH
    Low - 16 samples
    Medium - 22 samples
    High - 43 samples
    Veryhigh - 71 samples
*/
#define INSPECT_DOF_QUALITY_VERYHIGH

/*
    Controls DoF blur radius.
    Too high of a value might cause noise in final DoF result.
*/
#define INSPECT_DOF_RADIUS 10.0f

/*
    Controls DoF highlight boost amount.
    Too high of a value might cause incorrect colors in final DoF result.
*/
#define INSPECT_DOF_HIGHLIGHT_GAIN 0.4f