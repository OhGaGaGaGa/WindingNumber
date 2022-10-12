from vtkmodules.all import *

input_mixed = sys.argv[1]
output_tet = sys.argv[2]

reader = vtkUnstructuredGridReader()
reader.SetFileName(input_mixed)
reader.Update()

ugrid: vtkUnstructuredGrid = reader.GetOutput()
tetraFilter = vtkDataSetTriangleFilter()
tetraFilter.SetInputData(ugrid)
tetraFilter.Update()
tet = tetraFilter.GetOutput()

writer = vtkUnstructuredGridWriter()
writer.SetFileName(output_tet)
writer.SetInputData(tet)
writer.SetFileVersion(42)
writer.Write()
