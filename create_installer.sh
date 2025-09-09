#!/bin/bash

# Create installer script for ChaorusFlangos
echo "Creating ChaorusFlangos installer..."

# Create temporary directory
mkdir -p /tmp/ChaorusFlangos_Installer/Contents/Resources
mkdir -p /tmp/ChaorusFlangos_Installer/Contents/Resources/VST3
mkdir -p /tmp/ChaorusFlangos_Installer/Contents/Resources/Components
mkdir -p /tmp/ChaorusFlangos_Installer/Contents/Resources/VST

# Copy plugins
cp -R Builds/MacOSX/build/Release/ChaorusFlangos.vst3 /tmp/ChaorusFlangos_Installer/Contents/Resources/VST3/
cp -R Builds/MacOSX/build/Release/ChaorusFlangos.component /tmp/ChaorusFlangos_Installer/Contents/Resources/Components/
cp -R Builds/MacOSX/build/Release/ChaorusFlangos.vst /tmp/ChaorusFlangos_Installer/Contents/Resources/VST/

# Create installer script
cat > /tmp/ChaorusFlangos_Installer/Contents/Resources/postinstall << 'EOF'
#!/bin/bash

# Install VST3
if [ -d "/tmp/ChaorusFlangos_Installer/Contents/Resources/VST3/ChaorusFlangos.vst3" ]; then
    cp -R "/tmp/ChaorusFlangos_Installer/Contents/Resources/VST3/ChaorusFlangos.vst3" "/Library/Audio/Plug-Ins/VST3/"
    echo "VST3 installed successfully"
fi

# Install AudioUnit
if [ -d "/tmp/ChaorusFlangos_Installer/Contents/Resources/Components/ChaorusFlangos.component" ]; then
    cp -R "/tmp/ChaorusFlangos_Installer/Contents/Resources/Components/ChaorusFlangos.component" "/Library/Audio/Plug-Ins/Components/"
    echo "AudioUnit installed successfully"
fi

# Install VST2
if [ -d "/tmp/ChaorusFlangos_Installer/Contents/Resources/VST/ChaorusFlangos.vst" ]; then
    cp -R "/tmp/ChaorusFlangos_Installer/Contents/Resources/VST/ChaorusFlangos.vst" "/Library/Audio/Plug-Ins/VST/"
    echo "VST2 installed successfully"
fi

echo "ChaorusFlangos installation complete!"
echo "Please restart your DAW to use the plugin."
EOF

chmod +x /tmp/ChaorusFlangos_Installer/Contents/Resources/postinstall

# Create package info
cat > /tmp/ChaorusFlangos_Installer/Contents/Resources/package_info.plist << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleIdentifier</key>
    <string>com.chaorusflangos.plugin</string>
    <key>CFBundleName</key>
    <string>ChaorusFlangos</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
</dict>
</plist>
EOF

# Build the package
pkgbuild --root /tmp/ChaorusFlangos_Installer/Contents/Resources \
         --scripts /tmp/ChaorusFlangos_Installer/Contents/Resources \
         --identifier com.chaorusflangos.plugin \
         --version 1.0 \
         --install-location / \
         ChaorusFlangos_Installer.pkg

echo "Installer created: ChaorusFlangos_Installer.pkg"
echo "Users can double-click this file to install the plugin!"
