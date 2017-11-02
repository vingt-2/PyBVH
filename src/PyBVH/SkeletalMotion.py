import PyBVH.cSkeletalMotion as csk
import numpy as np
class Transform:
	

class SkeletalMotion:
	def __init__(self, c_instance):
		self.c_instance = c_instance
		self.is_valid = False

	def GetName(self):
		return csk.getName(self.c_instance)

	def GetFrameCount(self):
		return csk.getFrameCount(self.c_instance)
		
	def SetNormalizedScale(self):
		return csk.setNormalizedScale(self.c_instance, 1.0)
	
	def SetNormalizedScaleMultiplied(self, scale):
		return csk.setNormalizedScale(self.c_instance, float(scale))
	
	def GetJointPositions(self, frame_index, add_root=False):
		return csk.getJointPositions(self.c_instance, frame_index, 1 if add_root else 0)
	
	def GetCumulativeTransforms(self, frame_index, add_root=False):
		return csk.getCumTransform(self.c_instance, frame_index, 1 if add_root else 0)
		
	def GetSamplingRate(self):
		return csk.getSamplingRate(self.c_instance)