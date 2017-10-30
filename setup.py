from setuptools import setup, Extension

pybvh_module = Extension('PyBVH.cSkeletalMotion', sources = ['src/pybvh_module.cpp','src/skeletalMotion.cpp','src/bvh_importer.cpp' ], include_dirs = ['./include'], library_dirs = [], libraries = [])

setup (name = 'PyBVH',
        version = '0.1',
        description = 'BVH loader for Python',
        packages = [ 'PyBVH' ],
        package_dir = { '' : 'src' },
        ext_modules = [pybvh_module])