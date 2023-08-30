import { Component, OnInit } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';
import { BackendService } from 'src/app/backend/backend.service';
import { DroneInfo, DroneState } from 'src/app/backend/interfaces/client';
import { SocketHelperService } from 'src/app/backend/socket-helper.service';
import { ConnectDronesDialogComponent } from './connect-drones-dialog/connect-drones-dialog.component';
import { HelpDialogComponent } from './help-dialog/help-dialog.component';
import { SoftwareUpdateDialogComponent } from './software-update-dialog/software-update-dialog.component';

@Component({
    selector: 'app-main-page',
    templateUrl: './main-page.component.html',
    styleUrls: ['./main-page.component.scss'],
})
export class MainPageComponent implements OnInit {

    public numberOfDrones: number;
    public disableConnection: boolean;
    public logsEnabled: boolean;
    public logs: string[];
    public takeOffEnable: boolean;
    public landReturnEnable: boolean;
    public updateEnable: boolean;

    constructor(
        private backend: BackendService,
        private dialog: MatDialog,
        private socketHelper: SocketHelperService
    ) {
        this.logs = new Array<string>();
        this.logsEnabled = false;
        this.disableConnection = false;
        this.takeOffEnable = false;
        this.landReturnEnable = false;
        this.updateEnable = true;
    }

    public ngOnInit() {
        this.socketHelper.socket.on(
            'logs',
            (logs: string) => this.onLogReception(logs),
        );
    }

    private onLogReception(log: string) {
        this.logs.unshift(log);
        if(this.logs.length >= 20) {
            this.logs.pop();
        }
    }
  
    public openConnectDrones(): void {
        const dialogRef = this.dialog.open(ConnectDronesDialogComponent, {
            width: '350px',
            data: {drones: this.numberOfDrones},
        });

    }

    public softwareDialog(): void {
        const dialogRef = this.dialog.open(SoftwareUpdateDialogComponent, {
            width: '350px',
            data: {droneId: null, file: null},
        });
    }

    public openHelp(): void {
        this.dialog.open(HelpDialogComponent, { width: '400px' });
    }

    public takeOffDrone(): void {
        this.backend.takeOffDrone();
    }

    public landDrone(): void {
        this.backend.landDrone();
    }

    public returnToBase(): void {
        this.backend.returnToBase();
    }

    public updateButtonEnabling(drones: DroneInfo[]): void {
        let standby = false;
        let flying = false;
        let crashed = false;
        drones.forEach( drone => {
            standby = (drone.State == DroneState.Standby || standby);
            crashed = (drone.State == DroneState.Crashed || crashed);
            flying = (
                drone.State == DroneState.InMission ||
                drone.State == DroneState.Landing ||
                drone.State == DroneState.ReturnToBase ||
                drone.State == DroneState.TakeOff ||
                flying
            );
        });
        this.takeOffEnable = standby;
        this.landReturnEnable = flying;
        this.updateEnable = (!flying && !crashed) || !drones.length;
        this.disableConnection = flying;
    }
}
