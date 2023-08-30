# Crazysystem/Drone

---
**NOTE**

We recommend to use our ./run.sh at all times! The instructions here 'should' work, but we don't guarantee it.

Nous recommandons d'utilisez notre script ./run.sh tout le temps! Les instructions ici 'devraient' fonctionner, mais nous ne pouvons rien garantir!

---

## Run dockerfile / Rouler le dockerfile

first, build the container with: / D'abord, construisez le container:<br>
`docker build -t crazysystem_simulation . --network host --build-arg FILES_TO_COPY_SIM=/sim --build-arg FILES_TO_COPY_LIBRARY=/tools`<br>

Then, you can run the container using X11Docker (make sure you have it installed): / Ensuite, vous pouvez executer le container avec X11Docker (vérifiez bien que vous l'avez installé):<br>
`x11docker $(docker images -q crazysystem_simulation) --hostdisplay --hostnet --user=RETAIN --clipboard`<br>

Then, in another terminal (in Crazysystem/Drone directory), run this line to see the simulation: / Ensuite dans un autre terminal (dans le répertoire Crazysystem/Drone), executez cette ligne pour voir la simulation:<br>
`docker exec -it $(docker container ls -l -q) /bin/bash -c "cd /root/sim/; argos3 -c experiments/crazyflieSim.argos"`<br>

## How to flash drone / Comment téléverser sur le drone

### English
All you need to do is go to the base of the Crazysystem project and run: ./run.sh flash all <i>`path to device`</i> where the path to device is for the radio. For example, it looks like: /dev/bus/usb/... <br>

NOTE: When you run the command, make sure that the crazyflie radio is connected and that only ONE drone is in bootloader mode (hold the power button for about 2 seconds).<br>

### Français
Tout ce qu'il suffit à faire, c'est d'executer la commande suivante à la racine du projet crazysystem: ./run.sh flash all <i>`Chemin de la radio`</i> avec le chemin qui ressemble à cela: /dev/bus/usb/... <br>

NOTE: Quand vous executez la commande, assurez-vous que la radio crazyflie est connecté et que seulement UN SEUL drone soit en mode "bootloader" (gardez le bouton allumer enfoncer pendant à peu près 2 secondes).<br>
