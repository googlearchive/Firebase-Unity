#!/usr/bin/env

# This script exports the current plugin as a standalone asset that is easy to import in your project
# By simply merging it with your "Plugins" folder

# Remove the current export directory
rm -rf ../export

# Make an export folder inside the main directory
mkdir ../export

# Copy the current empty-project/Assets/Plugins directory to the export directory for editing
cp -r ../empty-project/Assets/Plugins ../export

# Remove the Android permissions file so that it doesn't overwrite existing permissions for a project
rm ../export/Plugins/Android/AndroidManifest.xml

# Remove all meta files (You don't want your plugin overwriting generated meta files for the Android/iOS/Edit etc plugins)
rm -rf *.meta
