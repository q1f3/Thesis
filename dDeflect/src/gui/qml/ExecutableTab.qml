import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.1
import jsk.components 1.0


Component{
    id: execTab
    Tab {
        title: "Executable"
        visible:true
        //property var methodsModel: applicationManager.archType === applicationManager.X86 ? applicationManager.x86methods : applicationManager.x64methods
        property var methodsModel: applicationManager.currMethods
        property var handlersModel: applicationManager.currHandlers
        property var tablicaComboBoxow: []
        property bool ss: true
        ColumnLayout{
            id:cl
            anchors.fill: parent
            //anchors.margins: 12
            anchors.topMargin: addButton.height/2
            spacing: 10
            RowLayout{
                Layout.alignment: Qt.AlignLeft
                width: 500
                height: 50
                spacing: 10
                Button{
                    id: addButton
                    visible: linCombo.checked
                    text:"Click to add a method."
                    onClicked: {
                        comboboxesmobel.append({"name":"kuba"})
                        tablicaComboBoxow.push(cbDelegate);
                        console.log(tablicaComboBoxow)
                        applicationManager.insertNewToList("");
                    }
                }
                Button{
                    id: save

                    text:"Save"
                    onClicked: {
                        console.log("Save?")
                        applicationManager.saveClicked()
                    }
                }
                Button{
                    id: apply
                    visible: linbuttons.checked || winbuttons.checked
                    text:"Apply"
                    onClicked: {
                        console.log("ATTEMPT TO SECURE AN EXEC!")
                        applicationManager.secureClicked();
                    }
                }
            }
            RowLayout{
                focus: true
                z: 1000
                Rectangle{
                    width: 500
                    height: 50
                    Loader{
                        id: radioLoader
                        anchors.fill: parent
                        sourceComponent: applicationManager.state===1 ? winCombo : linCombo

                    }
                }
                Component{
                    id: linCombo
                    DynamicRadioButtons{
                        id:linbuttons
                    }
                }
                Component{
                    id: winCombo
                    DynamicRadioWin{
                        id:winbuttons
                    }
                }

            }

            ListView {
                id: lv
                visible: true
                Layout.alignment: Qt.AlignLeft

                anchors.fill: parent
                anchors.topMargin: (addButton.height*3/2+lv.spacing)*2
                model: comboboxesmobel
                delegate: cbDelegate
                Component {
                    id: cbDelegate

                    Item{

                        height: 50
                        width: parent.width
                        RowLayout{
                            height: 50
                            width: parent.width
                            ComboBox {
                                id: methodCombo
                                currentIndex: 2
                                model: methodsModel
                                //model: applicationManager.x86methods
                                width: 200
                                textRole: "name"
                                onCurrentIndexChanged: {
                                    console.log(index);
                                    if(ss)
                                        applicationManager.changeList(methodCombo.currentText, handlerCombo.currentText, index)
                                }

                            }
                            ComboBox {
                                id: handlerCombo
                                visible: methodsModel[methodCombo.currentIndex].returns
                                model: handlersModel
                                //model: applicationManager.x86methods
                                width: 200
                                textRole: "name"
                                onCurrentIndexChanged: {
                                    console.log(index);
                                    if(ss)
                                        applicationManager.changeList(methodCombo.currentText, handlerCombo.currentText, index)
                                }

                            }
//                            Loader {

//                                Component{
//                                    id: handlerCombo

//                                }

//                                Component{
//                                    id: emptyId
//                                    Text{
//                                        width: 200
//                                        text:"No handler possible."
//                                    }
//                                }
//                                sourceComponent: methodsModel[methodCombo.currentIndex].returns ? (methodsModel[methodCombo.currentIndex].isThread === true ? c2 : handlerCombo) :emptyId

//                            }
                        }

                    }

                }
            }
            ListModel {
                id: comboboxesmobel
            }
            ListModel {
                     id: model
                     ListElement { name: "Banana"; color: "Yellow" }
                     ListElement { name: "Apple"; color: "Green" }
                     ListElement { name: "Coconut"; color: "Brown" }
                 }
        }
    }
}
