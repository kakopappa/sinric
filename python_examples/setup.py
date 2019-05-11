from setuptools import setup

VERSION = "0.0.1"

with open('README.rst', 'r') as f:
    long_description = f.read()
setup(
     name="pysinric",
    version=VERSION,
    author="dazzHere",
    author_email="dhanushdazz@gmail.com",
    description="A python package for your sinric alexa skill",
    long_description=long_description,
    url="https://github.com/kakopappa/sinric",
    packages=['sinric'],
    install_requires=['websocket_client'],
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 2",
        "Operating System :: OS Independent",
    ]
)