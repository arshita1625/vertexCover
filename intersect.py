import math
class Vertex(object):
    def __init__ (self, x, y):
        self.x = float(x)
        self.y = float(y)
        
    def __str__ (self):
        return '(' + str(self.x) + ',' + str(self.y) + ')'
    
    def __eq__(self, other):
        return hasattr(other, 'x') and hasattr(other, 'y') and self.x == other.x and self.y == other.y

    def __hash__(self):
         return hash((self.x, self.y))
    
class Line(object):
    def __init__ (self, src, dest):
        self.src = src
        self.dest = dest

    def __str__(self):
        return str(self.src) + '-->' + str(self.dest)

class Street:
    def __init__(self, name, list_segments):
        self.name=name
        self.list_segments=list_segments

    def __str__(self):
        lines = ', '.join(map(str, self.list_segments))
        return f"{self.name} --> {lines}"
    
def determinant(x1, y1, x2, y2, x3, y3, x4, y4):
    return (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)

def cal_inter_points(x1, y1, x2, y2, x3, y3, x4, y4, det):
    xnum = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4))
    xden = det
    xcoor = xnum / xden

    ynum = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)
    yden = det
    ycoor = ynum / yden

    return xcoor, ycoor

def intersect(line1, line2):
    x1, y1 = line1.src.x, line1.src.y
    x2, y2 = line1.dest.x, line1.dest.y
    x3, y3 = line2.src.x, line2.src.y
    x4, y4 = line2.dest.x, line2.dest.y

    det = determinant(x1, y1, x2, y2, x3, y3, x4, y4)

    if det == 0:
        if x2 == x3 and y2 == y3:
            return Vertex(x2, y2)
        return None

    coordinate_x, coordinate_y = cal_inter_points(x1, y1, x2, y2, x3, y3, x4, y4, det)

    within_line1=point_between_points(line1.src.x, coordinate_x, line1.dest.x) and point_between_points(line1.src.y, coordinate_y, line1.dest.y)
    within_line2=point_between_points(line2.src.x, coordinate_x, line2.dest.x) and point_between_points(line2.src.y, coordinate_y, line2.dest.y)
   
    if within_line1 and within_line2:
        return Vertex(coordinate_x, coordinate_y)

    return None

def point_between_points(x, q, z):
    return (x <= q <= z) or (z <= q <= x)

    
def cross_product(point1, point2, point3):
    diff_y1 = point2.y - point1.y
    diff_x2 = point3.x - point2.x
    diff_x1 = point2.x - point1.x
    diff_y2 = point3.y - point2.y
    return diff_y1 * diff_x2 - diff_x1 * diff_y2
    
def partial_overlap(line1, line2):
    if cross_product(line1.src, line1.dest, line2.src) == 0 and cross_product(line1.src, line1.dest, line2.dest) == 0:
        if point_between_points(line1.src.x, line2.src.x, line1.dest.x) or point_between_points(line1.src.x, line2.dest.x, line1.dest.x) or \
           point_between_points(line2.src.x, line1.src.x, line2.dest.x) or point_between_points(line2.src.x, line1.dest.x, line2.dest.x):
            return True
    return False

def point_lies_on_segment(start, mid, end, th=1e-10):

    x_var = min(start.x, end.x) - th <= mid.x <= max(start.x, end.x) + th
    y_var = min(start.y, end.y) - th <= mid.y <= max(start.y, end.y) + th

    is_collinear = abs((mid.x - start.x) * (end.y - start.y) - (end.x - start.x) * (mid.y - start.y)) <= th

    return x_var and y_var and is_collinear

def find_intersections(street_info):
    inter = set()
    for st in street_info:
        for segment in st.list_segments:
            lines = [line for street in street_info if street != st for line in street.list_segments]
            
            for lne in lines:
                if partial_overlap(segment, lne):
                    to_add = {segment.src, segment.dest, lne.src, lne.dest}
                    inter.update(to_add - {None})
                else:
                    intersection = intersect(segment, lne)
                    if intersection:
                        inter.add(intersection)
    
    return inter

def compute_dist(point1, point2):
    change_in_x = point2.x - point1.x
    change_in_y = point2.y - point1.y
    dist = change_in_x**2 + change_in_y**2
    return dist**0.5

def normalize_coordinate(value):
    if value == -0.0:
        return 0.0
    return value

