import sys
from PyQt5.QtWidgets import QApplication, QWidget, QGroupBox, QVBoxLayout, QHBoxLayout, QRadioButton, QLabel, QPlainTextEdit, QMessageBox, QPushButton

# 已建立链接并认证
name_1 = [10, 11, 12]
# 已建立链接但未认证
name_2 = [20, 21, 22]

class MyWindow(QWidget):

    def __init__(self):
        super().__init__()
        self.init_ui()



    def init_ui(self):
        # 设置Widget大小以及固定宽高
        self.setFixedSize(900, 700)

        #最外层的垂直布局，包含两部分：提示语和交流区域
        container = QVBoxLayout()

        #--------提示语--------
        #hobby_0 = QGroupBox("提示语：")#已建立链接但未认证为红色，已建立链接并且已认证为绿色:
        #todo:
        #把背景颜色去掉，加上类别字体颜色
        #lable = QLabel("提示语：")
        #lable1 = QLabel("已建立链接并认证:",self)
        lable1 = QLabel(f'''已建立链接并认证的有: \n   {name_1}''',self)
        lable2 = QLabel(f'''已建立链接但未认证的有: \n   {name_2}''',self)
        lable1.setStyleSheet("QLabel{color:rgb(0,102,0,255);font-size:20px;font-weight:normal;font-family:Arial;}")
        lable2.setStyleSheet("QLabel{color:rgb(153,0,0,255);font-size:20px;font-weight:normal;font-family:Arial;}")


        #--------交流区域--------
        hobby_box = QGroupBox("交流区域：")
        # h_layout 保证交流对象和消息框是平行摆放
        h_layout = QHBoxLayout()

        # --------交流对象--------
        hobby_11 = QGroupBox("交流对象：")
        hobby_1 = QVBoxLayout()
        #todo:使用循环语句建立交流对象的垂直列表
        btn1 = QPushButton("10")
        btn2 = QPushButton("11")
        btn3 = QPushButton("12")
        btn1.setStyleSheet("QPushButton{background-color:rgb(153,255,153)}QPushButton:hover{background-color:rgb(0,102,0)}")
        btn2.setStyleSheet("QPushButton{background-color:rgb(153,255,153)}QPushButton:hover{background-color:rgb(0,102,0)}")
        btn3.setStyleSheet("QPushButton{background-color:rgb(153,255,153)}QPushButton:hover{background-color:rgb(0,102,0)}")
        btn_press1.clicked.connect(self.btn_press1_clicked)
        btn_press2.clicked.connect(self.btn_press2_clicked)
        btn_press3.clicked.connect(self.btn_press3_clicked)
        # for i in name_1():
        #     btn[i] = QPushButton(name_1[i])
        #     btn[i].setStyleSheet("background-color : green")
        # 添加到hobby_1中
        hobby_1.addWidget(btn1)
        hobby_1.addWidget(btn2)
        hobby_1.addWidget(btn3)
        # 添加到hobby_11中
        hobby_11.setLayout(hobby_1)


        # --------交流框--------
        hobby_2 = QPlainTextEdit()
        hobby_2.setPlaceholderText("请输入要发送的信息（不可为空）")
       # hobby_2.resize(300, 300)
        #todo:设置抽屉

        #--------发送按钮--------
        hobby_3 = QPushButton("发送")

        #把三个小块平行放到交流区域中
        h_layout.addWidget(hobby_11)
        h_layout.addWidget(hobby_2)
        h_layout.addWidget(hobby_3)
        # 把h_layout添加到hobby_box中
        hobby_box.setLayout(h_layout)

        #把两个区域添加到容器中
        #container.addWidget(hobby_0)
        #container.addWidget(lable)
        container.addWidget(lable1)
        container.addWidget(lable2)
        container.addWidget(hobby_box)

        # 设置窗口显示的内容是最外层容器
        self.setLayout(container)

if __name__ == "__main__":
    app = QApplication(sys.argv)

    win = MyWindow()
    win.show()

    app.exec()