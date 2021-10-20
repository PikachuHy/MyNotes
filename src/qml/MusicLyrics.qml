import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

Rectangle {
    id: musicLyrics
    required property MediaPlayer mediaPlayer
    required property string musicName
    anchors.fill: parent
    Rectangle {
        id: diskImage
        anchors.top: musicNameText.bottom
        width: parent.width < parent.height ? parent.width : parent.height
        height: parent.width < parent.height ? parent.width : parent.height

        Image {
            source: "qrc:/icon/disk_600x600.jpeg"
            anchors.fill: parent
        }
        RotationAnimation on rotation {
            id: diskImageRotation
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 3600 * 2
        }

        Connections {
            target: mediaPlayer
            onPlaybackStateChanged: {
                if (mediaPlayer.playbackState == MediaPlayer.PlayingState) {
                    diskImageRotation.resume()
                } else {
                    diskImageRotation.pause()
                }
            }
        }
    }

    Text {
        id: musicNameText
        width: parent.width
        text: musicLyrics.musicName

        Connections {
            target: mediaPlayer
            function onMusicNameChanged() {
                musicNameText.text = mediaPlayer.musicName
            }
        }
    }
    Rectangle {
        width: parent.width
        height: 64
        anchors.bottom: parent.bottom
        Column {
            anchors.fill: parent
            Slider {
                id: slider
                width: parent.width
                height: 32
                from: 0
                to: 100
                value: 0
                onMoved: {
                    mediaPlayer.position = value
                }

                Connections {
                    target: mediaPlayer
                    function onPositionChanged(position) {
                        slider.value = position
                        slider.to = mediaPlayer.duration
                    }
                }
            }
            Item {
                width: parent.width
                height: 32
                Item {
                    width: parent.width
                    Item {
                        id: leftItem
                        width: 20
                    }
                    Item {
                        id: rightItem
                        width: 20
                        anchors.right: parent.right
                    }
                    Text {
                        id: currentTime
                        anchors.left: leftItem.right
                        text: "00:00"
                    }
                    Text {
                        anchors.right: rightItem.left
                        id: totalTime
                        text: "00:00"
                    }
                    Connections {
                        id: time
                        target: mediaPlayer
                        function format(time) {

                            time = time / 1000
                            time = Math.floor(time)
                            if (time < 10) {
                                return "00:0" + time
                            }

                            if (time < 60) {
                                return "00:" + time
                            }

                            var m = time / 60
                            m = Math.floor(m)
                            var s = time % 60
                            var ret = m
                            if (m < 10) {
                                ret = "0" + m
                            }
                            ret = ret + ":"
                            if (s < 10) {
                                ret = ret + "0" + s
                            } else {
                                ret = ret + s
                            }
                            return ret
                        }
                        function onPositionChanged(position) {
                            currentTime.text = time.format(position)
                            totalTime.text = time.format(mediaPlayer.duration)
                        }
                    }
                }

                Row {
                    spacing: 30
                    anchors.horizontalCenter: parent.horizontalCenter
                    Image {
                        width: 32
                        height: 32
                        source: "qrc:/icon/left-circle_64x64.png"
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                mediaPlayer.playPrev()
                            }
                        }
                    }
                    Image {
                        id: playOrPauseIcon
                        width: 32
                        height: 32
                        source: "qrc:/icon/play-circle_64x64.png"
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (mediaPlayer.playbackState == MediaPlayer.PlayingState) {
                                    playOrPauseIcon.source = "qrc:/icon/play-circle_64x64.png"
                                    mediaPlayer.pause()
                                } else {
                                    playOrPauseIcon.source = "qrc:/icon/pause-circle_64x64.png"
                                    mediaPlayer.play()
                                }
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
                                mediaPlayer.playNext()
                            }
                        }
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        slider.value = mediaPlayer.position
        slider.to = mediaPlayer.duration
        if (mediaPlayer.playbackState == MediaPlayer.PlayingState) {
            diskImageRotation.running = true
        } else {
            diskImageRotation.running = false
        }
        if (mediaPlayer.playbackState == MediaPlayer.PlayingState) {
            playOrPauseIcon.source = "qrc:/icon/pause-circle_64x64.png"
        } else {
            playOrPauseIcon.source = "qrc:/icon/play-circle_64x64.png"
        }
    }
}
