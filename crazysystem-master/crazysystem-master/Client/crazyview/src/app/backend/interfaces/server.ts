export interface ServDroneInfo {
    id: number;
    batteryLevel: number;
    speed: number;
    state: number;
}

export interface ServPoint {
    coordX: number;
    coordY: number;
}

export interface ServRangingDeckInfo {
    id: number;
    front: number;
    back: number;
    left: number;
    right: number;
}

export interface ServDronePosition {
    id: number,
    position: ServPoint,
}
