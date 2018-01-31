#!/bin/bash

ASTYLE=`which astyle`
ASTYLE_ARGS="--style=google --indent=spaces=2 --indent-modifiers 
	--pad-oper --pad-comma --pad-header --unpad-paren -k3
	--convert-tabs --attach-return-type --add-braces --recursive"

$ASTYLE $ASTYLE_ARGS "src/cluster/*.cpp"
$ASTYLE $ASTYLE_ARGS "src/cluster/*.h"
$ASTYLE $ASTYLE_ARGS "src/evolution/*.cpp"
$ASTYLE $ASTYLE_ARGS "src/evolution/*.h"
$ASTYLE $ASTYLE_ARGS "test/*.cpp"
