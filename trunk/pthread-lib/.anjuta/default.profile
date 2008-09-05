<?xml version="1.0"?>
<anjuta>
    <plugin name="API Help" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-devhelp:AnjutaDevhelp"/>
    </plugin>
    <plugin name="Subversion" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-subversion:Subversion"/>
    </plugin>
    <plugin name="Debugger" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-debug-manager:DebugManagerPlugin"/>
    </plugin>
    <plugin name="Macro Plugin" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-macro:MacroPlugin"/>
    </plugin>
    <plugin name="Terminal" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-terminal:TerminalPlugin"/>
    </plugin>
</anjuta>
