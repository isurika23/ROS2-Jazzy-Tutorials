from setuptools import find_packages, setup

package_name = 'tests'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='isurika',
    maintainer_email='gunasekaratiu.22@uom.lk',
    description='Task 05_Tests package',
    license='Apache-2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'chatter = tests.chatter:main',
            'listener = tests.listener:main',
        ],
    },
)
