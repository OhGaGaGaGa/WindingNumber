from vtkmodules.all import *

input_mesh = sys.argv[1]
output_mesh = sys.argv[2]

reader = vtkUnstructuredGridReader()
reader.SetFileName(input_mesh)
reader.Update()

writer = vtkUnstructuredGridWriter()
writer.SetFileVersion(42)
writer.SetFileName(output_mesh)
writer.SetInputData(reader.GetOutput())
writer.Write()
