import { Component, OnInit, Output, EventEmitter } from '@angular/core';
import { DroneInfo, DroneState, DroneStateString } from 'src/app/backend/interfaces/client';
import { ServDroneInfo } from 'src/app/backend/interfaces/server';
import { SocketHelperService } from 'src/app/backend/socket-helper.service';

@Component({
    selector: 'app-drone-info',
    templateUrl: './drone-info.component.html',
    styleUrls: ['./drone-info.component.scss'],
})
export class DroneInfoComponent implements OnInit {

    @Output() newDronesInfoEvent = new EventEmitter<DroneInfo[]>();
    
    public numberOfDrones: number;
    public drones: DroneInfo[];
    public readonly displayedColumns: string[] = ['id','batteryLevel', 'state', 'speed'];
    public lastUpdate: number;
    public readonly droneStateStrings = Object.values(DroneStateString); 

    constructor(
        private socketHelper: SocketHelperService,
    ) {
        this.drones = new Array<DroneInfo>();
        this.numberOfDrones = this.drones.length;
    }

    public ngOnInit(): void {
        this.socketHelper.socket.on(
            'drone-info',
            (dronesInfo: ServDroneInfo) => this.onInfo(dronesInfo),
        );
    }

    private onInfo(dronesInfo: ServDroneInfo): void {
        const drone = this.socketHelper.convertDronesInfos(dronesInfo);
        let tempDrones = new Array<DroneInfo>();
        this.drones.forEach(existingDrone => {
            tempDrones[existingDrone.Id-1] = existingDrone;
        });
        tempDrones[drone.Id-1] = drone;
        this.drones = tempDrones;
        this.numberOfDrones = this.drones.length;
        this.lastUpdate = Date.now();
        this.newDronesInfoEvent.emit(this.drones);
    }
}
