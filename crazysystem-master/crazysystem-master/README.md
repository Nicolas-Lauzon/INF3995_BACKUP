# ENGLISH

## Installation/First time use

<b>Our program works with the Stem Ranging Bundle from Bitcraze (https://store.bitcraze.io/products/stem-ranging-bundle).</b>
<br>
<br>
To use our system, first make sure that you have x11Docker installed (https://github.com/mviereck/x11docker) and Docker.<br>
For Docker, make sure that your user is in the Docker group and that Docker is running properly.<br>
<br>
Once you have Docker and x11Docker installed, please follow these steps:<br>
 1. Install the latest version our custom drone firmware on <b>ALL</b> of your drones. To do that, first check on which usb port the 
 crazyradio PA is plugged into with this command: `lsusb`. Then, you can run this command at the root of this project:<br>
 `./run.sh flash all /dev/bus/usb/.../...`. Please replace the `/dev/bus/usb/.../...` with the info that `lsusb` showed you.<br>
 NOTE: Please run the command for one drone at a time. <br>
 2. Once all of the drones have our custom firmware uploaded, you can run `./run.sh all /dev/usb/.../...`. Give it a few minutes to launch, 
 then you should see our webpage appear here: https://localhost:4200. If you want to see the webpage on every device on your local network, you just need to change a variable 
 in this file: `crazysystem/Client/src/environments/environment.ts`. In the `environment.ts` file, change the `LOCAL_IP` value to your local IP address.
 You can find that address by doing `ip a`.

## Crazysystem

Welcome to Crazysystem, a semi-autonomous drone project.<br>

The system is simple: tell a cluster of drones to scan an area up to 100 square meters and let the user see it on a basic web page from a phone, tablet or computer.<br>

Our client (web based) is coded in Angular 11. Our server is coded in Python, and the drones (and simulation) are coded in C/C++.<br>

To run the whole project, please read the section "How to use ./run.sh".<br>

For more info on each section (client, server, drone), read the README in each section.<br>

### How to use ./run.sh

NOTE: Please make sure that you have docker installed with your current user in the user group.<br>
You might need to give yourself access to the script with chmod.<br>

The script can automatically run everything. Here are the possible commands: <br>
./run.sh all <i>`path to device`</i><br>
./run.sh sim<br>
./run.sh sim test<br>
./run.sh client<br>
./run.sh client test<br>
./run.sh client server sim<br>
./run.sh client server drone <i>`path to device`</i><br>
./run.sh server sim<br>
./run.sh server drone <i>`path to device`</i><br>
./run.sh server test<br>
./run.sh flash all <i>`path to device`</i> <b> This command is the recommanded command! The simple one only flashes part of the code.</b><br>
./run.sh flash simple <i>`path to device`</i><br>
./run.sh test<br>

The path to device looks like this: /dev/bus/usb/...<br>

# FRANCAIS

## Installation/Première utilisation

<b>Notre logiciel fonctionne seulement avec le kit Stem Ranging Bundle de Bitcraze (https://store.bitcraze.io/products/stem-ranging-bundle).</b>
<br>
<br>
Pour utiliser notre système, veuillez installer x11Docker (https://github.com/mviereck/x11docker) ainsi que Docker.<br>
Pour Docker, assurez-vous que votre utilisateur se trouve dans le group Docker et que Docker fonctionne correctement.<br>
<br>
Quand vous avez x11Docker et Docker d'installer, veuillez suivre ces instructions:<br>
 1. Installez la dernière version du code embarqué sur <b>TOUT</b> vos drones. Pour cela, il faut d'abord trouver le port usb où se trouve 
 la crazyradio PA en faisant la commande `lsusb`. Ensuite, vous pouvez lancer la commande suivante à la racine de ce projet:<br>
 `./run.sh flash all /dev/bus/usb/.../...`. Veuillez remplacer la partie `/dev/bus/usb/.../...` par les informations données par la commande `lsusb`.<br>
 NOTE: Il faut executer la commande pour chaque drone individuellement.<br>
 2. Quand tous les drones ont la dernière version du code embarqué, vous pouvez executer la commande suivante:<br>
 `./run.sh all /dev/bus/usb/.../...`. Attendez quelques minutes pour que tout se lance, puis vous pourriez aller voir la page web à l'adresse suivante: 
 https://localhost:4200. Si vous voulez pouvoir voir la page web de n'importe quel appareil sur votre réseau local, veuillez changer une variable 
 dans le fichier `crazysystem/Client/src/environments/environment.ts`. Il suffit de changer la variable `LOCAL_IP` à votre adresse IP local. 
 Vous pouvez trouver votre adresse IP en faisant `ip a`.

## Crazysystem

Bienvenue sur Crazystem, un projet de drones semi-autonome.<br>

Le systeme est assez simple: controller un ensemble de drones pour scanner un espace jusqu'à 100 mètres carrés. L'utilisateur pourra alors voir le résultat sur une page web depuis un téléphone, tablette ou ordinateur.<br>

Notre client (sur le web) est codé en Angular 11. Notre serveur est en Python et les drones (et simulation) sont en C/C++.<br>

Pour executer tout le projet, lire la section "Comment utiliser le ./run.sh".<br>

Pour plus d'information sur chaque section (client, serveur et drone), veuillez lire les README dans chaque section.<br>

### Comment utiliser le ./run.sh

NOTE: Assurez-vous d'avoir Docker d'installer et que votre utilisateur courant soit dans le groupe necessaire pour utiliser docker correctement.<br>
Vous devriez peut-etre avoir besoin de vous donner acces au script avec chmod.<br>

Le script peut tout faire rouler automatiquement. Voici toutes les commandes possible:<br>
./run.sh all <i>`Chemin de la radio`</i><br>
./run.sh sim<br>
./run.sh sim test<br>
./run.sh client<br>
./run.sh client server sim<br>
./run.sh client server drone <i>`Chemin de la radio`</i><br>
./run.sh server sim<br>
./run.sh flash all<i>`Chemin de la radio`</i> <b> Cette commande est la bonne a utilisé! Le simple flash qu'une partie du code.</b> <br>
./run.sh flash simple<i>`Chemin de la radio`</i><br>

Le chemin de la radio ressemble à cela: /dev/bus/usb/...<br>
