import sys
from PyQt5.QtWidgets import QApplication, QGraphicsView, QGraphicsScene, QGraphicsEllipseItem, QGraphicsLineItem, QWidget, QPushButton
from PyQt5.QtCore import Qt, QPointF
from PyQt5.QtGui import QPainter, QPen, QBrush, QPolygonF, QColor
from PyQt5.QtWidgets import QHBoxLayout
import boolOp

class PolygonDrawer(QGraphicsView):
    def __init__(self, grid_size=20):
        super().__init__()
        self.grid_size = grid_size
        self.scene = QGraphicsScene(self)
        self.setScene(self.scene)
        self.setRenderHint(QPainter.Antialiasing)

        self.clip_polygon_flag = True
        self.vertices = []
        self.lines = []
        self.vertices_p2 = []
        self.lines_p2 = []

        for i in range(self.grid_size+1):
            self.scene.addLine(i*(400/self.grid_size), 0, i*(400/self.grid_size), 400, QPen(Qt.gray))
            self.scene.addLine(0, i*(400/self.grid_size), 400, i*(400/self.grid_size), QPen(Qt.gray))

        self.and_button = QPushButton(self)
        self.and_button.setText('AND')
        self.and_button.setGeometry(507,40,50,30)
        self.and_button.clicked.connect(self.and_operation)

        self.or_button = QPushButton(self)
        self.or_button.setText('OR')
        self.or_button.setGeometry(507,80,50,30)
        self.or_button.clicked.connect(self.or_operation)

        self.done_button = QPushButton(self)
        self.done_button.setText('DONE')
        self.done_button.setGeometry(507,120,50,30)
        self.done_button.clicked.connect(self.is_done)

        self.clear_button = QPushButton(self)
        self.clear_button.setText('CLEAR')
        self.clear_button.setGeometry(507,160,50,30)
        self.clear_button.clicked.connect(self.clear_scene)

    def is_done(self):
        if self.clip_polygon_flag:
            line = QGraphicsLineItem(self.vertices[0].x(), self.vertices[0].y(), self.vertices[-1].x(), self.vertices[-1].y())
            self.scene.addItem(line)
            self.lines.append(line)
            self.clip_polygon_flag = False
            self.scene.addPolygon(QPolygonF([vertex for vertex in self.vertices]), QPen(), QBrush(Qt.red))
            print("First Polygon Vertices:")
            for vertex in self.vertices:
                print(f"({vertex.x()/10}, {vertex.y()/10})")
        else:
            line = QGraphicsLineItem(self.vertices_p2[0].x(), self.vertices_p2[0].y(), self.vertices_p2[-1].x(), self.vertices_p2[-1].y())
            self.scene.addItem(line)
            self.lines_p2.append(line)
            brush_color = QColor(Qt.blue)
            brush_color.setAlpha(100)
            brush = QBrush(brush_color)
            self.scene.addPolygon(QPolygonF([vertex for vertex in self.vertices_p2]), QPen(), brush)
            print("Polygon Vertices:")
            for vertex in self.vertices_p2:
                print(f"({vertex.x()/10}, {vertex.y()/10})")


    def and_operation(self):
        py_clip = boolOp.Pg()
        py_clip.pts = [boolOp.Point(int(v.x()), int(v.y())) for v in self.vertices]

        py = boolOp.Pg()
        py.pts = [boolOp.Point(int(v.x()), int(v.y())) for v in self.vertices_p2]

        result = boolOp.andOperation(py_clip, py)
        self.vertices = []
        self.lines = []
        self.vertices_p2 = []
        self.lines_p2 = []
        print(result, result.pts[0].x, len(result.pts))
        for i in range(len(result.pts)):
            self.vertices.append(QPointF(result.pts[i].x,result.pts[i].y))
        self.scene.addPolygon(QPolygonF([vertex for vertex in self.vertices]), QPen(), QBrush(Qt.green))

    def or_operation(self):
        self.vertices_p2 = []
        self.lines_p2 = []

    def mousePressEvent(self, event):
        pos = self.mapToScene(event.pos())

        snapped_pos = QPointF(
            round(pos.x()/(400/self.grid_size))*(400/self.grid_size),
            round(pos.y()/(400/self.grid_size))*(400/self.grid_size)
        )
        if self.clip_polygon_flag:
            vertex = QGraphicsEllipseItem(snapped_pos.x()-1, snapped_pos.y()-1, 2, 2)
            vertex.setBrush(Qt.red)
            self.scene.addItem(vertex)
            self.vertices.append(QPointF(snapped_pos))

            if len(self.vertices) > 1:
                line = QGraphicsLineItem(self.vertices[-2].x(), self.vertices[-2].y(), snapped_pos.x(), snapped_pos.y())
                self.scene.addItem(line)
                self.lines.append(line)
        else:
            vertex = QGraphicsEllipseItem(snapped_pos.x()-1, snapped_pos.y()-1, 2, 2)
            vertex.setBrush(Qt.blue)
            self.scene.addItem(vertex)
            self.vertices_p2.append(QPointF(snapped_pos))

            if len(self.vertices_p2) > 1:
                line = QGraphicsLineItem(self.vertices_p2[-2].x(), self.vertices_p2[-2].y(), snapped_pos.x(), snapped_pos.y())
                self.scene.addItem(line)
                self.lines_p2.append(line)

    def clear_scene(self):
        self.scene.clear()
        self.vertices = []
        self.vertices_p2 = []
        self.lines = []
        self.lines_p2 = []
        self.clip_polygon_flag = True

        for i in range(self.grid_size+1):
            self.scene.addLine(i*(400/self.grid_size), 0, i*(400/self.grid_size), 400, QPen(Qt.gray))
            self.scene.addLine(0, i*(400/self.grid_size), 400, i*(400/self.grid_size), QPen(Qt.gray))

class PolyMap(QWidget):
    def __init__(self):
        super().__init__()
        self.polygon_drawer = PolygonDrawer(grid_size=20)
        layout = QHBoxLayout()
        layout.addWidget(self.polygon_drawer)
        self.setLayout(layout)
        self.resize(600, 500)
        self.setWindowTitle("PolyMap")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    main_window = PolyMap()
    main_window.show()
    sys.exit(app.exec_())
