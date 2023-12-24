import sys
from PyQt5.QtWidgets import QApplication, QGraphicsView, QGraphicsScene, QGraphicsEllipseItem, QGraphicsLineItem, QWidget, QPushButton, QFileDialog
from PyQt5.QtCore import Qt, QPointF
from PyQt5.QtCore import QRectF, QByteArray
from PyQt5.QtGui import QPainter, QPen, QImage, QImageWriter
from PyQt5.QtWidgets import QHBoxLayout
import myBoolOp
import time

def do_and(poly1, poly2):
    start_and = time.time()
    result = myBoolOp.andOperation(poly1, poly2)
    end_and = time.time()
    print("do_and time: ", end_and-start_and)
    return result

def do_or(poly1, poly2):
    start_or = time.time()
    result = myBoolOp.orOperation(poly1, poly2)
    end_or = time.time()
    print("do_or time: ", end_or-start_or)
    return result

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
        
        self.save_button = QPushButton(self)
        self.save_button.setText('SAVE')
        self.save_button.setGeometry(507,200,50,30)
        self.save_button.clicked.connect(self.save_scene)

        self.exit_button = QPushButton(self)
        self.exit_button.setText('EXIT')
        self.exit_button.setGeometry(507,240,50,30)
        self.exit_button.clicked.connect(self.exit)

    def draw_polygon(self, poly):
        length = len(poly)
        tmp = [poly[0].x, poly[0].y]
        if(self.clip_polygon_flag):
            color = Qt.blue
        else:
            color = Qt.red
        for i in range(len(poly)):
            # print("poly", poly[i].x, poly[i].y)
            if poly[(i+1)%length].x != -1:
                if(poly[i].x == -1):
                    continue
                # print("    ", poly[i].x, poly[i].y, poly[(i+1)%length].x, poly[(i+1)%length].y)
                self.vertices.append(QPointF(poly[i].x,poly[i].y))
                self.scene.addLine(poly[i].x, poly[i].y, poly[(i+1)%length].x, poly[(i+1)%length].y, QPen(color, 2, Qt.SolidLine))
            else:
                # print("    ", poly[i].x, poly[i].y, tmp[0], tmp[1])
                self.scene.addLine(poly[i].x, poly[i].y, tmp[0], tmp[1], QPen(color, 2, Qt.SolidLine))
                tmp = [poly[(i+2)%length].x, poly[(i+2)%length].y]

    def is_done(self):
        if self.clip_polygon_flag:
            self.scene.addLine(self.vertices[0].x(), self.vertices[0].y(), self.vertices[-1].x(), self.vertices[-1].y(), QPen(Qt.blue, 2, Qt.SolidLine))
            self.clip_polygon_flag = False
            # print("First Polygon Vertices:")
            # # for vertex in self.vertices:
            #     print(f"({vertex.x()/10}, {vertex.y()/10})")
        else:
            if self.vertices_p2 == []:
                pass
            else:
                self.scene.addLine(self.vertices_p2[0].x(), self.vertices_p2[0].y(), self.vertices_p2[-1].x(), self.vertices_p2[-1].y(), QPen(Qt.red, 2, Qt.SolidLine))
                # print("Second Polygon Vertices:")
                # for vertex in self.vertices_p2:
                #     print(f"({vertex.x()/10}, {vertex.y()/10})")
    
    def and_operation(self):
        poly1 = [myBoolOp.Point(v.x(), v.y()) for v in self.vertices]

        poly2 = [myBoolOp.Point(v.x(), v.y()) for v in self.vertices_p2]
        result = do_and(poly1, poly2)
        x_y_coordinates = [(point.x, point.y) for point in result]
        # for x, y in x_y_coordinates:
        #     print(f"({x}, {y}),")
        print("poly1")
        for v in self.vertices:
            print(f"({v.x()}, {v.y()}),")
        print("poly2")
        for v in self.vertices_p2:
            print(f"({v.x()}, {v.y()}),")
        

        self.vertices = []
        self.lines = []
        self.vertices_p2 = []
        self.lines_p2 = []
        if len(result) ==0:
            return
        self.clear_scene()
        self.draw_polygon(result)
        poly1 = result;
        poly2 = []
        self.clip_polygon_flag = False

    def or_operation(self):
        poly1 = [myBoolOp.Point(v.x(), v.y()) for v in self.vertices]
        poly2 = [myBoolOp.Point(v.x(), v.y()) for v in self.vertices_p2]
        result = do_or(poly1, poly2)
        # x_y_coordinates = [(point.x, point.y) for point in result]
        # for x, y in x_y_coordinates:
        #     print(f"({x}, {y}),")
        print("poly1")
        for v in self.vertices:
            print(f"({v.x()}, {v.y()}),")
        print("poly2")
        for v in self.vertices_p2:
            print(f"({v.x()}, {v.y()}),")
        self.vertices = []
        self.lines = []
        self.vertices_p2 = []
        self.lines_p2 = []
        if len(result) ==0:
            return
        self.clear_scene()
        self.draw_polygon(result)
        poly1 = result;
        poly2 = []
        self.clip_polygon_flag = False

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
                self.scene.addLine(self.vertices[-2].x(), self.vertices[-2].y(), snapped_pos.x(), snapped_pos.y(), QPen(Qt.blue, 2, Qt.SolidLine))

                
        else:
            vertex = QGraphicsEllipseItem(snapped_pos.x()-1, snapped_pos.y()-1, 2, 2)
            vertex.setBrush(Qt.red)
            self.scene.addItem(vertex)
            self.vertices_p2.append(QPointF(snapped_pos))

            if len(self.vertices_p2) > 1:
                self.scene.addLine(self.vertices_p2[-2].x(), self.vertices_p2[-2].y(), snapped_pos.x(), snapped_pos.y(), QPen(Qt.red, 2, Qt.SolidLine))

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

    def save_scene(self):
        image = QImage(400, 400, QImage.Format_ARGB32)
        image.fill(Qt.white)

        painter = QPainter(image)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setRenderHint(QPainter.SmoothPixmapTransform)

        source_rect = QRectF(0, 0, 400, 400)
        target_rect = QRectF(0, 0, 400, 400)
        
        self.scene.render(painter, target=target_rect, source=source_rect)
        painter.end()

        image_writer = QImageWriter("output.png")
        image_writer.setFormat(QByteArray(b"png"))
        
        image_writer.write(image)
        
    def exit(self):
        sys.exit(app.exec_())


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
