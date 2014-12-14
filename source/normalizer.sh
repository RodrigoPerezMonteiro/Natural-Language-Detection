sed '/^\s*$/d' < $1 								|
sed 's/[«»\n\r\t‚‘’–<>=—±²{}'\''\\$]//g'	| 
tr '[:upper:]' '[:lower:]' 							| 
tr -s '[:?!.]' '.' 										| 
sed 's/\.[^0-9]/\.\n/g'							|
sed -e 's/^[ \r\t]*//' 								|
sed '/^\s*$/d' 										|
sed 's/[;,] /,/g' 										|
sed 's/^.*$/<<&>/g' 								|
sed 's/[”“]/"/g'										|
sed 's/[()%:";,/-]/ & /g'							|
sed 's/\./ &/g'										|
sed 's/  */ /g'											|
sed 's/< */</g'										|
#tr -s '[^A-Za-z0-9/:,<>"()%?!.]' ' '	|
#pt
#sed 's/À/à/g'				|
#sed 's/Á/á/g'				|
#sed 's/Â/â/g'				|
#sed 's/Ã/ã/g'				|
#sed 's/É/é/g'				|
#sed 's/Ê/ê/g'				|
#sed 's/Í/í/g'					|
#sed 's/Ó/ó/g'				|
#sed 's/Ô/ô/g'				|
#sed 's/Õ/õ/g'				|
#sed 's/Ú/ú/g'				|
#sed 's/Ü/ü/g'				|
#sed 's/Ç/ç/g'				|
#sed 's/[ªº«»€]//g'		|
#de
#sed 's/Ä/ä/g'				|
#sed 's/Ö/ö/g'				|
#sed 's/Ü/ü/g'				|
#fr
#sed 's/È/è/g'				|
#sed 's/Ë/ë/g'				|
#sed 's/Î/î/g'					|
#sed 's/Ï/ï/g'					|
#sed 's/[Œœ]//g'			|
#sed 's/Ù/ù/g'				|
#sed 's/Û/û/g'				|
#sed 's/Ÿ/ÿ/g'				|
#it
#sed 's/Ì/ì/g'			> $2

#pt
sed 's/[ÀàÁáÂâÃã]/a/g'		|
sed 's/[ÉéÊê]/e/g'				|
sed 's/[Íí]/i/g'						|
sed 's/[ÓóÔôÕõ]/o/g'			|
sed 's/[ÚúÜü]/u/g'				|
sed 's/[Çç]/c/g'					|
sed 's/[ªº«»€]//g'				|
#de
sed 's/[Ää]/a/g'					|
sed 's/[Öö]/o/g'					|
sed 's/[Üü]/u/g'					|
#fr
sed 's/[ÈèËë]/e/g'				|
sed 's/[ÎîÏï]/i/g'					|
sed 's/[ÙùÛû]/u/g'				|
sed 's/[Ÿÿ]/y/g'					|
sed 's/[Œœ]//g'					|
#it
sed 's/ò/o/g' 						|
sed 's/[Ìì]/i/g'		> $2




