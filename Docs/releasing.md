# Releasing

## Creating a release
### Versioning

We use [semantic versioning](https://semver.org/). A summary is provided.

Version is numerical, following major.minor.patch. As in, "1.5.9".

Patch updates are bug fixes that don't break backwards compatibility.

Minor updates are features that don't break backwards compatibility.

Major updates break backwards compatibility.

In each case, when making a version update, decrement the less significant number(s) to 0. As in, 1.4.5 -> 1.5.0.

We do not use -beta suffixes as they are difficult to represent numerically in housekeeping data.

In the software, each version number is kept by a uint16 to fit in the housekeeping data.

### Backwards compatibility

The API must not be made incompatible with previous versions within a major version. If the API becomes backwards incompatible, a new major version should be made. This software does not provide and API in the traditonal sense. API will be taken instead to mean service formats and network protocol formats (for example, MPDU format, sync word). If a feature changes a service such that the format is changed, the major version must be updated.

### Release branches
Releases are manually tagged. Due to the difficulty of pushing software updates, CI/CD is not used in production. However, ground based development can still happen quickly and continuously.

A branch may be made off a release tag to fix bugs. Bugfix commits may be cherry-picked off master or master may cherry-pick bugfix commits. When a patch is ready to be released, create a new tag with the release number at that point and create a new release.

### Submodules
Ensure the submodule pointer is at the correct commit for this version. When downloading the source the current position of the submodule pointer must be used. This is the version that will be compiled in the binaries.

### Binaries
Build binaries for all the satellite configurations. Ensure the correct hardware is configured for each and any other settings. Set the value BOOTLOADER_LINKAGE to 1 the HL_sys_link.cmd. This will give the image bootloader linkage, so it may be uploaded to the satellite through the updater program.
Ensure the binaries have -O3 enabled.

Github provides a way to upload binaries with a release. Upload the compiled binary for each satellite.

## Using a release
Binary files are provided in a working state for the satellites currently deployed. Only binaries for the fullt integrated satellites are provided. Test satellites such as flatsats and engineering models may not be compatible with the binaries provided. A source download is available for manual building. Note that the compiled binary may differ slightly from the binary found on the releases page

### Uploading
Follow the instructions in the updater docs in the groundstation software. Ensure the satellite you are uploading to is the same as the binary specified. It is unknown if a wrong binary will brick the satellite. Don't try it in production.