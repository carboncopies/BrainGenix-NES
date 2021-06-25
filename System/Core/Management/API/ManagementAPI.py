###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import socket
import threading
import queue
import json
import select


from Core.VersionData import VersionNumber
from Core.VersionData import BranchVersion


'''
Name: Management API
Description: This file provides the socket based interface to the management api backend.
Date-Created: 2021-05-17
'''


class ManagementAPISocketServer(): # Creates A Class To Connect To The Management API #

    def __init__(self, Logger, MAPIConfig:dict, ZookeeperConfigDict:dict, ThreadManager:object): # This function initialializes sockets #

        # Get Config Params #
        self.Logger = Logger
        self.Port = MAPIConfig['Port'] # Get the port from the port config
        self.IPAddr = ZookeeperConfigDict['ZKHost'] # Get The IP Addr from the zoomeeper dict
        self.ThreadManager = ThreadManager

        # Create Socket Host Variable #
        self.Logger.Log('Creating Host Variable')
        self.SocketHost = (self.IPAddr, self.Port)

        # Bind To Port #
        self.Logger.Log('Binding To Host')
        self.Socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.Socket.bind(self.SocketHost)

        # Listen For Connections #
        self.Logger.Log('Management API Backend Listening For Incoming Connections')
        self.Socket.listen()

        # Create MAPI Thread #
        self.Logger.Log('Starting Management API Server Thread', 3)

        # Create Control Queue #
        self.Logger.Log('Creating Control Queue For ManagementAPISocketServer Thread', 2)
        self.ManagementAPIThreadControlQueue = queue.Queue()
        self.Logger.Log('Created Control Queue For ManagementAPISocketServer Thread', 1)

        # Create Thread #
        self.Logger.Log('Creating Thread For ManagementAPISocketServer Daemon', 2)
        self.Thread = threading.Thread(target=self.ManagementAPIThread, args=(self.ManagementAPIThreadControlQueue, ), name='mAPI Socket Server')
        self.Logger.Log('Created Control Queue For ManagementAPISocketServer Daemon', 1)

        # Start Thread #
        self.Logger.Log('Starting ManagementAPISocketServer Daemon', 2)
        self.Thread.start()
        self.Logger.Log('Started ManagementAPISocketServer Daemon', 1)

        # Add Thread To Control Manager #
        self.Logger.Log('Adding ManagementAPISocketServer To Thread Manager', 3)

        self.Logger.Log('Appending ManagementAPISocketServerControlQueue To Control Queue List', 2)
        self.ThreadManager.ControlQueues.append(self.ManagementAPIThreadControlQueue)
        self.Logger.Log('Appended ManagementAPISocketServerControlQueue To Control Queue List', 1)

        self.Logger.Log('Appending ManagementAPISocketServer Thread Object To ThreadManager Thread List', 2)
        self.ThreadManager.Threads.append(self.Thread)
        self.Logger.Log('Appended ManagementAPISocketServer Thread To ThreadManager Thread List', 1)


    def IndexCommands(self): # Creates An Index Of Commands For LS To Search Through #

        # Log Process Start #
        self.Logger.Log('Indexing Management API Commands', 4)

        # Create List Of Attributes #
        self.Logger.Log('Generating Raw List Of Attributes Exposed To Management API', 3)
        UnsortedList = dir(self.LFTM)
        HigherLevelAttr = self.LFTM

        # Recursive Filter #
        for AttributeNameString in UnsortedList:

            AttributeObject = getattr(HigherLevelAttr, AttributeNameString)
            







    # Load In External Commands #
    def LinkLFTM(self, LFTMInstance): # Link LFTM #

        # Log Link Start #
        self.Logger.Log('Linking LFTM Instance To APIServer')

        # Link LFTM #
        self.LFTM = LFTMInstance

        # Log Finish Method #
        self.Logger.Log('Linking Complete')


    def ExecuteCommand(self): # Runs The Command #

        # Extract Values From Command Dict #
        CommandCallStack = self.Command['CallStack']
        ArgumentsDictionary = self.Command['KeywordArgs']

        # Get Target Function #
        Layers = CommandCallStack.split('.')
        CommandFunction = self


        # Iterate Through Layers, Run Command Called #
        for LayerIndex in range(len(Layers)):

            try:
                # Run Command With Prefix Included (mAPI_[Command Name]) #
                if LayerIndex < (len(Layers) - 1):
                    CommandFunction = getattr(CommandFunction, Layers[LayerIndex])
                else:
                    CommandFunction = getattr(CommandFunction, 'mAPI_'+Layers[LayerIndex])

                # Run Function #
                CommandOutput = CommandFunction(ArgumentsDictionary)
                CommandName = CommandCallStack


            except Exception as ErrorString:

                # Format Return Error For Return To Client #
                CommandOutput = str(ErrorString)
                CommandName = 'Error'


        # Return Values #
        return CommandOutput, CommandName





    def ManagementAPIThread(self, ControlQueue): # Create A Thread Function For The Management API #

        # Enter Connection Accept Loop #
        while ControlQueue.empty():

            try:
                # Log That Server Awaiting Connections #
                self.Logger.Log(f'MAPI Server Awaiting Connections On Port: {self.Port}')

                # Set Socket Timeout #
                self.Logger.Log('Setting Socket Timeout', 2)
                self.Socket.settimeout(0.2)
                self.Logger.Log('Set Socket Timeout', 1)

                # Wait Accept Incoming Connections #
                self.Logger.Log('Entering Connection Accept Loop', 2)
                while ControlQueue.empty():

                    # Await Connection #
                    try:
                        self.Connection, self.ConnectionInformation = self.Socket.accept()
                        self.Logger.Log(f'Management API Recieved Connection From: {self.ConnectionInformation}', 7)

                        # Set Nonblocking #
                        self.Connection.setblocking(0)

                        # Exit Loop #
                        break

                    except socket.timeout:
                        pass


                # Enter Listening Loop To Recieve Commands #
                self.Logger.Log('Management API Server Awaiting Commands', 4)
                while ControlQueue.empty():


                    # Get Command From Client #
                    SocketReady = select.select([self.Connection], [], [], 1)
                    if SocketReady[0]:
                        self.Command = self.Connection.recv(65535)
                        self.Command = self.Command.decode()
                    else:
                        self.Command = None

                    # Check If Command Ready #
                    if self.Command != None:

                        # Convert To Dict From JSON #
                        try:
                            self.Command = json.loads(self.Command)
                        except Exception as e:
                            self.Logger.Log('Exception In Management Server JSONLOAD', 1)
                            self.Logger.Log(e)


                        # Check That Command Syntax Is Correct #
                        if str(type(self.Command)) != "<class 'dict'>":
                            CommandOutput = "INVALID DICTIONARY FORMAT"

                        if 'CallStack' not in self.Command:
                            CommandOutput = "COMMAND DOES NOT INCLUDE 'CallStack' FIELD. If using CLI, run 'scope (NES, ERS, STS)'"


                        # Check If Disconnect (Must be before SysName check to remain client agnostic) #
                        elif self.Command['CallStack'] == 'Disconnect':
                            self.Logger.Log('Client Initiated MAPI Disconnect, Connection Closed')
                            self.Connection.close()
                            break


                        # More Command Syntax Checks #
                        elif 'SysName' not in self.Command:
                            CommandOutput = "COMMAND DOES NOT INCLUDE 'SysName' FIELD"

                        elif 'KeywordArgs' not in self.Command:
                            CommandOutput = "COMMAND DOES NOT INCLUDE 'KeywordArgs' FIELD"

                        elif self.Command['SysName'] != 'NES':
                            CommandOutput = "INVALID VALUE FOR 'SysName' FIELD"

                        # Run System Command #
                        else:
                            CommandOutput, CommandName = self.ExecuteCommand()



                        # Encode JSON Output #
                        Response = {"Name" : CommandName, "Content" : CommandOutput}
                        ResponseString = json.dumps(Response)
                        ResponseByteString = ResponseString.encode()

                        # Send Output #
                        self.Connection.send(ResponseByteString)



            except Exception as E:

                self.Logger.Log(E)
                self.Logger.Log('Exception within APIServer, Restarting Server!')

                try:

                    self.Connection.close()

                except Exception as E:

                    # Log Exception #
                    self.Logger.Log(f'Exception In Management APIServer: {E}')

        # Exit Message #
        self.Logger.Log('Management API Socket Server Shutting Down', 4)

        self.Logger.Log('Closing Management API Server Socket', 2)
        self.Quit()
        self.Logger.Log('mAPI Server Socket Closed', 1)

        self.Logger.Log('Finished Shutting Down Management API Socket Server', 3)


    def Quit(self): # Release The Socket #

        # Close The Socket #
        self.Socket.close()


    # ListAttribute Command #
    def mAPI_ls(self, ArgumentsDictionary):


        ##################################################################################################
        ## NOTE: WE SHOULD HAVE THE COMMAND TREE BE PRE-INDEXED TO PROVIDE THE BEST LS FUNCTIONALLITY   ##
        ## THIS SHOULD MAKE USE OF A NESTED DICT APPROACH, INCLUDING THE COMMANDS WITH THE mAPI_ PREFIX ##
        ##################################################################################################


        # Check Command Validity #
        if 'Path' not in ArgumentsDictionary:
            ArgumentsDictionary.update({'Path':''})


        # Get Attributes From Arguments #
        TargetPath = 'LFTM'

        if ArgumentsDictionary['Path'] != '':
            TargetPath += f'.{ArgumentsDictionary["Path"]}'
        else:
            pass


        # Get Attributes #
        AttrTarget = self

        if TargetPath != "":

            for TargetPathName in TargetPath.split('.'):
                AttrTarget = getattr(AttrTarget, TargetPathName)

        Attributes = dir(AttrTarget)

        # Sort Attributes #
        OutAttr = ['__']
        
        for Attr in Attributes:
            Exists = False
            Allow = False
            Directory = True
            Attr4 = []
            while Directory:
                if (not Allow) and ('__' not in str(Attr)) and ("mAPI_" in str("Attr")):
                    for Attr2 in dir(Attr):
                        if "mAPI_" in str(Attr2):
                            Allow = True
                    Attr4 = Attr
                mAPI = True
                if Allow:
                    for Attr5 in Attr4:
                        for Attr6 in dir(Attr5):
                            if "mAPI_" in str(Attr6):
                                mAPI = False
                else:
                    Directory = False
                Dir = False
                if Allow:
                    Attr7 = Attr4
                    Attr4 = []
                    for Attr8 in dir(Attr7):
                        Attr4.Append(Attr8)
                for Dir2 in (Attr4):
                    if dir(Dir2) != []:
                        Dir = True
                if Dir == True:
                    Directory = False
            if mAPI == False:
                for Object in OutAttr:
                    for Attr3 in Attributes:
                        if Object == Attr3:
                            Exists = True
                if Exists == False:
                    OutAttr.append(Attr)
        del OutAttr[0]

        # Return Output #
        return str(OutAttr)


    def mAPI_Help(self, ArgumentsDictionary): # Provides Basic About The BGCLI #

        HelpMessage = 'This system provides a functional management interface to the BrainGenix system. Please use "ls Path=[path here]" to find commands, and use help Path=[path here] to find more information about a specific command. Please note that commands are seperated via a ".", so if calling command b nested under a, it would be "a.b".'

        return HelpMessage


    def mAPI_Version(self, ArgumentsDictionary): # Provide Versioning Information #

        # Set Full Branch Version #
        if BranchVersion == 'dev':
            BranchVersionType = 'Development'
        elif BranchVersion == 'rel':
            BranchVersionType = 'Release'
        else:
            BranchVersionType = 'ERR-UNKNOWN'

        # Construct Version Information #
        Version = f'BrainGenix-NES (Neuron Emulation System) [{BranchVersionType} Branch] [{VersionNumber}]'
        
        # Return Version String #
        return Version


    def mAPI_License(self, ArgumentsDictionary): # Provides Basic License Information As Command

        LicenseText = '''                                 Apache License
                            Version 2.0, January 2004
                            http://www.apache.org/licenses/

    TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

    1. Definitions.

        "License" shall mean the terms and conditions for use, reproduction,
        and distribution as defined by Sections 1 through 9 of this document.

        "Licensor" shall mean the copyright owner or entity authorized by
        the copyright owner that is granting the License.

        "Legal Entity" shall mean the union of the acting entity and all
        other entities that control, are controlled by, or are under common
        control with that entity. For the purposes of this definition,
        "control" means (i) the power, direct or indirect, to cause the
        direction or management of such entity, whether by contract or
        otherwise, or (ii) ownership of fifty percent (50%) or more of the
        outstanding shares, or (iii) beneficial ownership of such entity.

        "You" (or "Your") shall mean an individual or Legal Entity
        exercising permissions granted by this License.

        "Source" form shall mean the preferred form for making modifications,
        including but not limited to software source code, documentation
        source, and configuration files.

        "Object" form shall mean any form resulting from mechanical
        transformation or translation of a Source form, including but
        not limited to compiled object code, generated documentation,
        and conversions to other media types.

        "Work" shall mean the work of authorship, whether in Source or
        Object form, made available under the License, as indicated by a
        copyright notice that is included in or attached to the work
        (an example is provided in the Appendix below).

        "Derivative Works" shall mean any work, whether in Source or Object
        form, that is based on (or derived from) the Work and for which the
        editorial revisions, annotations, elaborations, or other modifications
        represent, as a whole, an original work of authorship. For the purposes
        of this License, Derivative Works shall not include works that remain
        separable from, or merely link (or bind by name) to the interfaces of,
        the Work and Derivative Works thereof.

        "Contribution" shall mean any work of authorship, including
        the original version of the Work and any modifications or additions
        to that Work or Derivative Works thereof, that is intentionally
        submitted to Licensor for inclusion in the Work by the copyright owner
        or by an individual or Legal Entity authorized to submit on behalf of
        the copyright owner. For the purposes of this definition, "submitted"
        means any form of electronic, verbal, or written communication sent
        to the Licensor or its representatives, including but not limited to
        communication on electronic mailing lists, source code control systems,
        and issue tracking systems that are managed by, or on behalf of, the
        Licensor for the purpose of discussing and improving the Work, but
        excluding communication that is conspicuously marked or otherwise
        designated in writing by the copyright owner as "Not a Contribution."

        "Contributor" shall mean Licensor and any individual or Legal Entity
        on behalf of whom a Contribution has been received by Licensor and
        subsequently incorporated within the Work.

    2. Grant of Copyright License. Subject to the terms and conditions of
        this License, each Contributor hereby grants to You a perpetual,
        worldwide, non-exclusive, no-charge, royalty-free, irrevocable
        copyright license to reproduce, prepare Derivative Works of,
        publicly display, publicly perform, sublicense, and distribute the
        Work and such Derivative Works in Source or Object form.

    3. Grant of Patent License. Subject to the terms and conditions of
        this License, each Contributor hereby grants to You a perpetual,
        worldwide, non-exclusive, no-charge, royalty-free, irrevocable
        (except as stated in this section) patent license to make, have made,
        use, offer to sell, sell, import, and otherwise transfer the Work,
        where such license applies only to those patent claims licensable
        by such Contributor that are necessarily infringed by their
        Contribution(s) alone or by combination of their Contribution(s)
        with the Work to which such Contribution(s) was submitted. If You
        institute patent litigation against any entity (including a
        cross-claim or counterclaim in a lawsuit) alleging that the Work
        or a Contribution incorporated within the Work constitutes direct
        or contributory patent infringement, then any patent licenses
        granted to You under this License for that Work shall terminate
        as of the date such litigation is filed.

    4. Redistribution. You may reproduce and distribute copies of the
        Work or Derivative Works thereof in any medium, with or without
        modifications, and in Source or Object form, provided that You
        meet the following conditions:

        (a) You must give any other recipients of the Work or
            Derivative Works a copy of this License; and

        (b) You must cause any modified files to carry prominent notices
            stating that You changed the files; and

        (c) You must retain, in the Source form of any Derivative Works
            that You distribute, all copyright, patent, trademark, and
            attribution notices from the Source form of the Work,
            excluding those notices that do not pertain to any part of
            the Derivative Works; and

        (d) If the Work includes a "NOTICE" text file as part of its
            distribution, then any Derivative Works that You distribute must
            include a readable copy of the attribution notices contained
            within such NOTICE file, excluding those notices that do not
            pertain to any part of the Derivative Works, in at least one
            of the following places: within a NOTICE text file distributed
            as part of the Derivative Works; within the Source form or
            documentation, if provided along with the Derivative Works; or,
            within a display generated by the Derivative Works, if and
            wherever such third-party notices normally appear. The contents
            of the NOTICE file are for informational purposes only and
            do not modify the License. You may add Your own attribution
            notices within Derivative Works that You distribute, alongside
            or as an addendum to the NOTICE text from the Work, provided
            that such additional attribution notices cannot be construed
            as modifying the License.

        You may add Your own copyright statement to Your modifications and
        may provide additional or different license terms and conditions
        for use, reproduction, or distribution of Your modifications, or
        for any such Derivative Works as a whole, provided Your use,
        reproduction, and distribution of the Work otherwise complies with
        the conditions stated in this License.

    5. Submission of Contributions. Unless You explicitly state otherwise,
        any Contribution intentionally submitted for inclusion in the Work
        by You to the Licensor shall be under the terms and conditions of
        this License, without any additional terms or conditions.
        Notwithstanding the above, nothing herein shall supersede or modify
        the terms of any separate license agreement you may have executed
        with Licensor regarding such Contributions.

    6. Trademarks. This License does not grant permission to use the trade
        names, trademarks, service marks, or product names of the Licensor,
        except as required for reasonable and customary use in describing the
        origin of the Work and reproducing the content of the NOTICE file.

    7. Disclaimer of Warranty. Unless required by applicable law or
        agreed to in writing, Licensor provides the Work (and each
        Contributor provides its Contributions) on an "AS IS" BASIS,
        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
        implied, including, without limitation, any warranties or conditions
        of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
        PARTICULAR PURPOSE. You are solely responsible for determining the
        appropriateness of using or redistributing the Work and assume any
        risks associated with Your exercise of permissions under this License.

    8. Limitation of Liability. In no event and under no legal theory,
        whether in tort (including negligence), contract, or otherwise,
        unless required by applicable law (such as deliberate and grossly
        negligent acts) or agreed to in writing, shall any Contributor be
        liable to You for damages, including any direct, indirect, special,
        incidental, or consequential damages of any character arising as a
        result of this License or out of the use or inability to use the
        Work (including but not limited to damages for loss of goodwill,
        work stoppage, computer failure or malfunction, or any and all
        other commercial damages or losses), even if such Contributor
        has been advised of the possibility of such damages.

    9. Accepting Warranty or Additional Liability. While redistributing
        the Work or Derivative Works thereof, You may choose to offer,
        and charge a fee for, acceptance of support, warranty, indemnity,
        or other liability obligations and/or rights consistent with this
        License. However, in accepting such obligations, You may act only
        on Your own behalf and on Your sole responsibility, not on behalf
        of any other Contributor, and only if You agree to indemnify,
        defend, and hold each Contributor harmless for any liability
        incurred by, or claims asserted against, such Contributor by reason
        of your accepting any such warranty or additional liability.

    END OF TERMS AND CONDITIONS

    APPENDIX: How to apply the Apache License to your work.

        To apply the Apache License to your work, attach the following
        boilerplate notice, with the fields enclosed by brackets "[]"
        replaced with your own identifying information. (Don't include
        the brackets!)  The text should be enclosed in the appropriate
        comment syntax for the file format. We also recommend that a
        file or class name and description of purpose be included on the
        same "printed page" as the copyright notice for easier
        identification within third-party archives.

    Copyright [yyyy] [name of copyright owner]

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
        '''

        return LicenseText


    def mAPI_TestAPI(self, ArgumentsDictionary): # Returns A Test String #

        # You should get this refrerence... (Look it up) #
        return "but most of all, samy is my hero" 
