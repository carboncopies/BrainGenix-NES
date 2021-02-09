###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Builtin Commands
Description: This does exactly what it sounds like, which is handle some builtin commands for the system.
Date-Created: 2021-01-29
'''

class BG(): # Standard Commands #

    '''
    This class contains a set of functions used by the ZKCLI as a builtin set of commands for the core systems to BG.
    You can interact with these by calling 'BG.help' in the CLI.
    That will produce a list of commands and a breif description of what they do.
    *Please Don't Call This Unless You Know What You're Doing!*
    '''

    def __init__(self, Zookeeper, SystemTelemetryLeader): # Initialize System #

        '''
        Initializes the variables in the BG class.
        *Please Don't Call This Unless You Know What You're Doing!*
        '''

        # Create Local Pointers #
        self.Zookeeper = Zookeeper
        self.SysTelLeader = SystemTelemetryLeader



    #########################
    # Define Commands Below #
    #########################

    def help(self):
        helpstring = '''
Built-in Commands:
    - BG.WhoAmI: returns the name of the node the server runs on
    - BG.WhoIsLeader: returns name of leader node, should match output of BG.whoami
    - BG.Version: Prints Version Information about BG.
    - BG.License: Outputs the license text. BG is licensed under the Apache 2 Open Source License.
    - BG.ListNodes: outputs a list of all currently connected nodes
    - BG.NodeCPU --Node [NODE Hostname]: Lists cpu usage percentages for this node. You must pass --Node followed by the node hostname.
    - BG.ClusterCPU: Lists cpu info for the entire cluster
    - BG.NodeMemory --Node [NODE Hostname]: Displays Stats about this node's memory usages/totals. You must pass --Node followed by the node hostname.
    - BG.ClusterMemory: Displays stats about the entire cluster's memory usages/totals
    - BG.help: this help message
        '''
        return helpstring


    def whoami(self):
        NodeName = self.Zookeeper.Name
        return NodeName

    def version(self):
        return 'BrainGenix V0.0.3 Development,Unstable'

    def license(self):
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
   limitations under the License.'''

        return LicenseText

    def whoisleader(self):
        NodeName = self.Zookeeper.ZookeeperConnection.get('/BrainGenix/System/Leader')[0].decode()
        return NodeName


    def listnodes(self):
        Nodes = self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')
        OutString = ''
        for Node in Nodes:
            OutString += f'{Node}\n'
        OutString = OutString[:-1]
        return OutString
    

    def nodecount(self):
        Nodes = self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')
        return str(len(Nodes))


    def nodecpu(self, CLA:dict):
        try:
            NodeName = CLA.get('--Node')
        except:
            return 'Invalid Argument, Please check your Arguments.'

        try:
            info = self.SysTelLeader.Info.get(NodeName)
            if info == None:
                return f'Node {NodeName} does not exist! Please check your arguments.'
        except:
            return f'Node {NodeName} does not exist! Please check your arguments.'
        
        CPUInfo = info.get('CPUUsage')
        avgcpu = sum(CPUInfo)/len(CPUInfo)
        Outstring = f'Average CPU for node {NodeName}: {round(avgcpu)}%\n'
        for CpuCore in enumerate(CPUInfo):
            Outstring += f'    Core {CpuCore}: {CPUInfo[CpuCore]}%\n'
        Outstring = Outstring[:-1]
        return Outstring


    def clustercpu(self):
        CPUInfo = []

        for Node in self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/System/Nodes'):
            info = self.SysTelLeader.Info.get(Node)
            CPUInfo += info.get('CPUUsage')

        avgcpu = sum(CPUInfo)/len(CPUInfo)
        Outstring = f'Average CPU for cluster: {round(avgcpu)}%\n'

        for CpuCore in range(len(CPUInfo)):
            Outstring += f'    Core {CpuCore}: {CPUInfo[CpuCore]}%\n'
        Outstring = Outstring[:-1]

        return Outstring


    def nodememory(self, CLA:dict):
        try:
            NodeName = CLA.get('--Node')
        except:
            return 'Invalid Argument, Please check your Arguments.'

        try:
            Info = self.SysTelLeader.Info.get(NodeName)
            if Info == None:
                return f'Node {NodeName} does not exist! Please check your arguments.'
        except:
            return f'Node {NodeName} does not exist! Please check your arguments.'

        Outstring = f'Node {NodeName} Is Using {Info.get("RAMPercent")}% of Physical Memory\n'
        Outstring += f'    - {Info.get("RAMUsage")} Bytes Used\n'
        Outstring += f'    - {Info.get("RAMFree")} Bytes Free\n'
        Outstring += f'    - {int(Info.get("RAMUsage")) + int(Info.get("RAMFree"))} Bytes Total\n'

        Outstring += f'Node Using {Info.get("SWAPPercent")}% of Swap\n'
        Outstring += f'    - {Info.get("SWAPUsage")} Bytes Used\n'
        Outstring += f'    - {Info.get("SWAPFree")} Bytes Free\n'
        Outstring += f'    - {int(Info.get("SWAPUsage")) + int(Info.get("SWAPFree"))} Bytes Total\n'

        return Outstring
    

    def clustermemory(self):

        RAMFreeTotal = 0
        RAMUsedTotal = 0
        RAMPercents = []

        SwapFreeTotal = 0
        SwapUsedTotal = 0
        SwapPercents = []

        for Node in self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/System/Nodes'):
            Info = self.SysTelLeader.Info.get(Node)
            RAMFreeTotal += Info.get('RAMFree')
            RAMUsedTotal += Info.get('RAMUsage')
            RAMPercents.append(Info.get('RAMPercent'))

            SwapFreeTotal += Info.get('SWAPFree')
            SwapUsedTotal += Info.get('SWAPUsage')
            SwapPercents.append(Info.get('SWAPPercent'))

        AVGRAM = sum(RAMPercents) / len(RAMPercents)
        AVGSWAP = sum(SwapPercents) / len(SwapPercents)

        Outstring = f'Cluster Memory Stats | RAM: {round(AVGRAM)}%, Swap: {round(AVGSWAP)}%\n'

        Outstring += f'    - {int(RAMUsedTotal)} Bytes RAM Used\n'
        Outstring += f'    - {int(RAMFreeTotal)} Bytes RAM Free\n\n'

        Outstring += f'    - {int(SwapUsedTotal)} Bytes Swap Used\n'
        Outstring += f'    - {int(SwapFreeTotal)} Bytes Swap Free\n'

        return Outstring

    def SyntaxError(self):
        return 'BG_COMMAND_ENGINE: COMMAND IS NOT RECOGNIZED AS VALID IN FORMAT'