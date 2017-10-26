from PyBVH.SkeletalMotion import *

def LoadBVH(file_path):
	skeletal_motion_c = csk.loadBVH(file_path)
	return SkeletalMotion(skeletal_motion_c)