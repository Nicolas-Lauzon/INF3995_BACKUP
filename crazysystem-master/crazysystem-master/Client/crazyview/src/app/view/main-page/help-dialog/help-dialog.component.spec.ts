import { ComponentFixture, TestBed } from '@angular/core/testing';
import { MatDialogRef } from '@angular/material/dialog';

import { HelpDialogComponent } from './help-dialog.component';

describe('HelpDialogComponent', () => {
    let component: HelpDialogComponent;
    let fixture: ComponentFixture<HelpDialogComponent>;
    let dialogRefSpy: jasmine.SpyObj<MatDialogRef<HelpDialogComponent>>;

    beforeEach(async () => {
        dialogRefSpy = jasmine.createSpyObj(
            'MatDialogRef',
            ['close'],
        );

        await TestBed.configureTestingModule({
            declarations: [ HelpDialogComponent ],
            providers: [
                { provide: MatDialogRef, useValue: dialogRefSpy },
            ],
        }).compileComponents();
    });

    beforeEach(() => {
        fixture = TestBed.createComponent(HelpDialogComponent);
        component = fixture.componentInstance;
        fixture.detectChanges();
    });

    it('should create', () => {
        expect(component).toBeTruthy();
    });

    it('should close dialog', () => {
        component.close();
        expect(dialogRefSpy.close).toHaveBeenCalled();
    });
});
