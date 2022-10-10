#======================================================================//
# This file is part of the BrainGenix-ERS Environment Rendering System //
#======================================================================//

'''
    Description: This tool is used to compile gltfs into bg scene graphs.
    Additonal Notes: None
    Date Created: 2021-11-12
'''

import pyassimp
import os
import yaml


def Main():

    # Load Asset Into Memory
    Scene = pyassimp.load("SourceAssets/Model.fbx")
    