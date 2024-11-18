bold=$(tput bold)
dim=$(tput dim)
normal=$(tput sgr0)
underline=$(tput smul)
blink=$(tput blink)
reverse=$(tput smso)
invis=$(tput invis)
fg_black=$(tput setaf 0)
fg_red=$(tput setaf 1)
fg_green=$(tput setaf 2)
fg_yellow=$(tput setaf 3)
fg_blue=$(tput setaf 4)
fg_magenta=$(tput setaf 5)
fg_cyan=$(tput setaf 6)
fg_white=$(tput setaf 7)
bg_black=$(tput setab 0)
bg_red=$(tput setab 1)
bg_green=$(tput setab 2)
bg_yellow=$(tput setab 3)
bg_blue=$(tput setab 4)
bg_magenta=$(tput setab 5)
bg_cyan=$(tput setab 6)
bg_white=$(tput setab 7)
echo -e "${bold}Bold${normal}\t\tNon-Bold"
echo -e "${dim}Dim${normal}\t\tNon-Dim"
echo -e "${underline}Underline${normal}\tNon-Underline"
echo -e "${blink}Blink${normal}\t\tNon-Blink"
echo -e "${reverse}Reverse${normal}\t\tNon-Reverse"
echo -e "${invis}Invisible${normal}\tNon-Invisible"
echo -e "${fg_black}Black${normal}\t\tDefault"
echo -e "${fg_red}Red${normal}\t\tDefault"
echo -e "${fg_green}Green${normal}\t\tDefault"
echo -e "${fg_yellow}Yellow${normal}\t\tDefault"
echo -e "${fg_blue}Blue${normal}\t\tDefault"
echo -e "${fg_magenta}Magenta${normal}\t\tDefault"
echo -e "${fg_cyan}Cyan${normal}\t\tDefault"
echo -e "${fg_white}White${normal}\t\tDefault"
echo -e "${bg_black}${fg_white}Black${normal}\t\tDefault"
echo -e "${bg_red}${fg_white}Red${normal}\t\tDefault"
echo -e "${bg_green}${fg_black}Green${normal}\t\tDefault"
echo -e "${bg_yellow}${fg_black}Yellow${normal}\t\tDefault"
echo -e "${bg_blue}${fg_white}Blue${normal}\t\tDefault"
echo -e "${bg_magenta}${fg_white}Magenta${normal}\t\tDefault"
echo -e "${bg_cyan}${fg_black}Cyan${normal}\t\tDefault"
echo -e "${bg_white}${fg_black}White${normal}\t\tDefault"

