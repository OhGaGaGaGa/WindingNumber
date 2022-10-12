from vtkmodules.all import *
import sys


input = sys.argv[1]
output = sys.argv[2]

reader = vtkUnstructuredGridReader()
reader.SetFileName(input)
reader.Update()

ugrid: vtkUnstructuredGrid = reader.GetOutput()
measure = vtkCellQuality()
measure.SetQualityMeasureToScaledJacobian()
measure.SetInputData(ugrid)
measure.Update()

varray = measure.GetOutput().GetCellData().GetArray("CellQuality")


for i in range(ugrid.GetNumberOfCells()):
    cell: vtkCell = ugrid.GetCell(i)
    flip = varray.GetValue(i) < 0
    if cell.GetCellType() == VTK_WEDGE:
        flip = not flip  # ?
    if flip:
        print(f"Cell {i} is flipped: {varray.GetValue(i)}")
        ids = [cell.GetPointId(j) for j in range(cell.GetNumberOfPoints())]
        if cell.GetCellType() == VTK_TETRA:
            ids[0], ids[1] = ids[1], ids[0]
        elif cell.GetCellType() == VTK_HEXAHEDRON:
            ids[1], ids[2], ids[3] = ids[3], ids[2], ids[1]
            ids[5], ids[6], ids[7] = ids[7], ids[6], ids[5]
        elif cell.GetCellType() == VTK_WEDGE:
            ids[1], ids[2] = ids[2], ids[1]
            ids[4], ids[5] = ids[5], ids[4]
        for j in range(cell.GetNumberOfPoints()):
            cell.GetPointIds().SetId(j, 0)
        ugrid.ReplaceCell(i, cell.GetNumberOfPoints(), ids)

writer = vtkUnstructuredGridWriter()
writer.SetFileName(output)
writer.SetInputData(ugrid)
writer.SetFileVersion(42)
writer.Write()
