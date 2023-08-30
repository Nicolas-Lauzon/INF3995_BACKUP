export interface DroneInfo {
    Id: number;
    BatteryLevel: number;
    Speed: number;
    State: DroneState;
}

export enum DroneState {
    Standby = 0,
    InMission = 1,
    Crashed = 2,
    TakeOff = 3,
    Landing = 4,
    ReturnToBase = 5
}

export enum DroneStateString {
    Standby = 'En attente',
    InMission = 'En mission',
    Crashed = 'Ecrasé',
    TakeOff = 'Décollage',
    Landing = 'Atterrissage',
    ReturnToBase = 'Retour à la base'
}
