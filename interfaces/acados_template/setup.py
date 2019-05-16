from setuptools import setup, find_packages

setup(name='acados_template',
   version='0.1',
   python_requires='>=3.5',
   description='A templating framework for acados',
   url='http://github.com/zanellia/acados',
   author='Andrea Zanelli',
   license='LGPL',
   packages = find_packages(),
   include_package_data = True,
   install_requires=[
      'jinja2',
      'numpy',
      'scipy',
      'casadi>=3.4.0'
   ],
   package_data={'': [
       'c_templates/main.in.c',
       'c_templates/Makefile.in',
       'c_templates/model.in.h',
       'c_templates/main.in.h',
       'c_templates/acados_solver.in.c',
       'c_templates/acados_solver_sfun.in.c',
       'c_templates/acados_solver.in.h',
       'c_templates/p_constraint.in.h',
       'c_templates/h_constraint.in.h',
       'c_templates/make_sfun.in.m',
       'acados_layout.json'
       ]},
   zip_safe=False
)
