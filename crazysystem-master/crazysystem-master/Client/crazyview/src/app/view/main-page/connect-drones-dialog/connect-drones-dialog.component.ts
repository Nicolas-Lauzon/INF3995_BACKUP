import { Component, Inject } from '@angular/core';
import { MatDialogRef, MAT_DIALOG_DATA } from '@angular/material/dialog';
import { BackendService } from 'src/app/backend/backend.service';

export interface ConnectDronesData {
    numberOfDrones: number;
}

@Component({
    selector: 'app-connect-drones-dialog',
    templateUrl: './connect-drones-dialog.component.html',
    styleUrls: ['./connect-drones-dialog.component.scss']
})
export class ConnectDronesDialogComponent {

    public isLoading: boolean;
    public errorDuringConnection: boolean;

    constructor(
        private dialogRef: MatDialogRef<ConnectDronesDialogComponent>,
        @Inject(MAT_DIALOG_DATA) public data: ConnectDronesData,
        private backend: BackendService,
    ) {
        this.isLoading = false;
        this.errorDuringConnection = false;
    }
  
    public close(): void {
        this.dialogRef.close();
    }

    public confirmClicked(): void {
        this.dialogRef.disableClose = true;
        let nbDrones = this.data.numberOfDrones;
        this.isLoading = true;
        if (nbDrones != null && nbDrones >= 2 && nbDrones <= 99) {
            this.backend.connectDrones(nbDrones).then(response => {
                if(response == '0') {
                    this.dialogRef.close();
                    this.errorDuringConnection = false;
                } else {
                    this.errorDuringConnection = true;
                }
                this.isLoading = false;
                this.dialogRef.disableClose = false;
            })
        }
    }

}
