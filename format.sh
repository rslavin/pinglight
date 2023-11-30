BLACK='30'
RED='31'
GREEN='32'
YELLOW='33'
BLUE='34'
WHITE='37'

NONE='00'
BOLD='01'
DULL='02'
UNDERLINE='04'

function format {
    MESSAGE=${1}
    COLOR=${2:-$WHITE}  # defaults to black if not specified
    STYLE=${3:-$NONE}   # defaults to none if not specified
    BG=${4:-$BLACK}     # defaults to black if not specified

    echo -e "\033[${STYLE};${COLOR}m${MESSAGE}\033[00m"
    return
}

