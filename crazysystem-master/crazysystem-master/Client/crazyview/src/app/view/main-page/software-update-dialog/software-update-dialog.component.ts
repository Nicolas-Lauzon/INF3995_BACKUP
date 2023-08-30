import { Component, Inject } from '@angular/core';
import { MatDialogRef, MAT_DIALOG_DATA } from '@angular/material/dialog';
import { BackendService } from 'src/app/backend/backend.service';

export interface SoftwareUpdateData {
    droneId: number;
    file: string;
}

@Component({
    selector: 'app-software-update-dialog',
    templateUrl: './software-update-dialog.component.html',
    styleUrls: ['./software-update-dialog.component.scss']
})
export class SoftwareUpdateDialogComponent {

    public isLoading: boolean;
    public errorDuringConnection: boolean;

    constructor(
        private dialogRef: MatDialogRef<SoftwareUpdateDialogComponent>,
        @Inject(MAT_DIALOG_DATA) public data: SoftwareUpdateData,
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
        this.isLoading = true;
        this.backend.softwareUpdate(this.data.droneId, "/home" + this.data.file + ".bin").then(response => {
            if(response == '0') {
                this.dialogRef.close();
                this.errorDuringConnection = false;
            } else {
                this.errorDuringConnection = true;
            }
            this.isLoading = false;
            this.dialogRef.disableClose = false;
        });
    }

}
