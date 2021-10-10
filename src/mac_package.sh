# 制作图标
work_path=$(cd `dirname $0`;pwd)

echo "work_path is $work_path"

if [ -d "icons.iconset" ];then
  echo "icons.iconset exist. delete it."
  rm -rf icons.iconset
fi

mkdir icons.iconset
sips -z 16 16     ../icon/notebook_128x128.png --out icons.iconset/icon_16x16.png
sips -z 32 32     ../icon/notebook_128x128.png --out icons.iconset/icon_16x16@2x.png
sips -z 32 32     ../icon/notebook_128x128.png --out icons.iconset/icon_32x32.png
sips -z 64 64     ../icon/notebook_128x128.png --out icons.iconset/icon_32x32@2x.png
sips -z 64 64     ../icon/notebook_128x128.png --out icons.iconset/icon_64x64.png
sips -z 128 128   ../icon/notebook_128x128.png --out icons.iconset/icon_64x64@2x.png
sips -z 128 128   ../icon/notebook_128x128.png --out icons.iconset/icon_128x128.png
sips -z 256 256   ../icon/notebook_128x128.png --out icons.iconset/icon_128x128@2x.png
sips -z 256 256   ../icon/notebook_128x128.png --out icons.iconset/icon_256x256.png
sips -z 512 512   ../icon/notebook_128x128.png --out icons.iconset/icon_256x256@2x.png
sips -z 512 512   ../icon/notebook_128x128.png --out icons.iconset/icon_512x512.png
sips -z 1024 1024   ../icon/notebook_128x128.png --out icons.iconset/icon_512x512@2x.png
iconutil -c icns icons.iconset -o Icon.icns
if [ ! -d "MyNotes.app/Contents/Resources" ];then
  mkdir MyNotes.app/Contents/Resources
fi
cp Icon.icns MyNotes.app/Contents/Resources/
cp ../Info.plist MyNotes.app/Contents/
echo "build dmg"
/Users/pikachu/Qt/5.15.2/clang_64/bin/macdeployqt ./MyNotes.app -dmg