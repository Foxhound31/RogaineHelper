package com.example.vad.myapplication;

import android.graphics.Point;
import android.view.View;

/**
 * Created by Vad on 19.04.2016.
 */
public class RHPoint {
    private boolean enabled;
    private int cost;
    private int number;
    private int x;
    private int y;
    View v;

    public RHPoint() {
        cost = 0;
        number = 0;
        x = 0;
        y = 0;
        v = null;
    }
    public RHPoint(int x, int y, View v) {
        cost = 0;
        number = 0;
        this.x = x;
        this.y = y;
        this.v = v;
    }

    public Point getPoint() {
        Point p = new Point(x, y);
        p.set(x, y);
        return p;
    }

    public View getView() {
        return v;
    }
}