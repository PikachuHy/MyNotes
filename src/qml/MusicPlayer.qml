import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

Item {
    id: musicPlayerRoot
    property bool playing: false
    property int currentIndex: 0
    width: 400
    height: 800

    MediaPlayer {
        id: mediaPlayer
        audioOutput: AudioOutput {}
        onPositionChanged: position => {
                               slider.value = position
                               slider.to = mediaPlayer.duration
                               mobileProcess.setProcess(
                                   position / mediaPlayer.duration)
                           }
        onPlaybackStateChanged: {
            if (playbackState === MediaPlayer.StoppedState
                    && musicPlayerRoot.playing) {
                musicPlayerRoot.playNext()
            }
        }
    }
    ColumnLayout {
        width: parent.width
        height: parent.height
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: ListModel {
                id: listModel
            }
            delegate: Rectangle {
                id: item
                width: musicPlayerRoot.width
                height: 32

                Text {
                    text: model.name
                    anchors.verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: {
                        musicPlayerRoot.play(model.path, model.name)
                        musicPlayerRoot.currentIndex = index
                    }

                    onEntered: {
                        if ($Controller.isDesktop()) {
                            item.color = "#eee"
                        }
                    }
                    onExited: {
                        if ($Controller.isDesktop()) {
                            item.color = "transparent"
                        }
                    }
                }
            }
        }

        Rectangle {
            visible: $Controller.isDesktop()
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                Image {
                    width: 32
                    height: 32
                    source: "qrc:/icon/left-circle_64x64.png"
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            musicPlayerRoot.playPrev()
                        }
                    }
                }
                Image {
                    id: desktopPlayStatusIcon
                    width: 32
                    height: 32
                    source: "qrc:/icon/play-circle_64x64.png"
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            musicPlayerRoot.playing = !musicPlayerRoot.playing
                        }
                    }
                }
                Image {
                    width: 32
                    height: 32
                    source: "qrc:/icon/right-circle_64x64.png"
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            musicPlayerRoot.playNext()
                        }
                    }
                }
            }
            Item {
                width: parent.width
                height: 32

                Text {
                    id: desktopMusicName
                    text: ""
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Rectangle {
                width: parent.width
                height: 32
                anchors.bottom: parent.bottom

                Slider {
                    id: slider
                    anchors.fill: parent
                    from: 0
                    to: 100
                    value: 0
                    onMoved: {
                        mediaPlayer.position = value
                    }
                }
            }
        }
        Rectangle {
            visible: $Controller.isMobile()
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            Component.onCompleted: {

            }
            // 这个进度条在安卓上无法显示，奇怪
            Rectangle {
                width: parent.width
                height: 32
                anchors.bottom: parent.bottom
                Rectangle {
                    id: mobileProcess
                    width: 0
                    height: parent.height
                    color: '#eee'
                    function setProcess(val) {
                        mobileProcess.width = val * parent.width
                    }
                }
            }
            Row {
                width: parent.width - 40
                height: parent.height
                leftPadding: 20
                Text {
                    width: parent.width - mobilePlayStatusIcon.width
                    anchors.verticalCenter: parent.verticalCenter
                    id: mobileMusicName
                    text: ""
                }

                Image {
                    id: mobilePlayStatusIcon
                    width: 32
                    height: 32
                    anchors.verticalCenter: parent.verticalCenter
                    source: "qrc:/icon/play-circle_64x64.png"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            musicPlayerRoot.playing = !musicPlayerRoot.playing
                        }
                    }
                }
            }
        }
    }

    onPlayingChanged: {
        if (playing) {
            mediaPlayer.play()
            desktopPlayStatusIcon.source = "qrc:/icon/pause-circle_64x64.png"
            mobilePlayStatusIcon.source = "qrc:/icon/pause-circle_64x64.png"
        } else {
            mediaPlayer.pause()
            desktopPlayStatusIcon.source = "qrc:/icon/play-circle_64x64.png"
            mobilePlayStatusIcon.source = "qrc:/icon/play-circle_64x64.png"
        }
    }
    function playNext() {
        var newIndex = (musicPlayerRoot.currentIndex + 1) % listModel.count
        var data = listModel.get(newIndex)
        console.log(musicPlayerRoot.currentIndex, newIndex, data)
        musicPlayerRoot.play(data.path, data.name)
        musicPlayerRoot.currentIndex = newIndex
    }
    function playPrev() {
        var newIndex = (musicPlayerRoot.currentIndex - 1 + listModel.count) % listModel.count
        var data = listModel.get(newIndex)
        console.log(musicPlayerRoot.currentIndex, newIndex, data)
        musicPlayerRoot.play(data.path, data.name)
        musicPlayerRoot.currentIndex = newIndex
    }

    function play(path, name) {
        if (path.length > 0) {
            console.log('play ', path)
            mediaPlayer.source = "file://" + path

            // 这个时候还拿不到值
            // slider.to = mediaPlayer.duration
            slider.value = 0
            desktopMusicName.text = name
            mobileMusicName.text = name
        }
        musicPlayerRoot.playing = true
        mediaPlayer.play()
    }
    function pause() {
        mediaPlayer.pause()
    }

    Component.onCompleted: {
        var basePath = $Controller.musicDataPath()
        var musics = $FileSystem.listDir(basePath)
        for (var i = 0; i < musics.length; i++) {
            listModel.append({
                                 "name": musics[i],
                                 "path": basePath + "/" + musics[i]
                             })
        }
    }
}
