package com.rogainer.rogainer;

/**
 * Created by Vad on 17.02.2017.
 * Class for storing application view options
 */

public final class ViewOptions {
    boolean mShowLabels;
    int mUnderlayer;    // TODO set type
    int mOpacity;
    boolean mShowNumbers;
    boolean mShowRoutes;

    public ViewOptions() {
        mShowLabels = true;
        mUnderlayer = 0;
        mOpacity = 100;
        mShowNumbers = true;
        mShowRoutes = true;
    }

    public ViewOptions(boolean showLabels, int underlayer, int opacity,
                       boolean showNumbers, boolean showRoutes)
    {
        mShowLabels = showLabels;
        mUnderlayer = underlayer; // TODO check for underlayer
        mOpacity = opacity;       // TODO check for opacity
        mShowNumbers = showNumbers;
        mShowRoutes = showRoutes;
    }

    public boolean getShowLabels() {
        return mShowLabels;
    }

    public void setShowLabels(boolean showLabels) {
        mShowLabels = showLabels;
    }

    public int getUnderlayer() {
        return mUnderlayer;
    }

    public void setUnderlayer(int underlayer) {
        mUnderlayer = underlayer;   // TODO check for underlayer
    }

    public int getOpacity() {
        return mOpacity;
    }

    public void setOpacity(int opacity) {
        mOpacity = opacity; // TODO check for opacity
    }

    public boolean getShowNumbers() {
        return mShowNumbers;
    }

    public void setShowNumbers(boolean showNumbers) {
        mShowNumbers = showNumbers;
    }

    public boolean getShowRoutes() {
        return mShowRoutes;
    }

    public void setShowRoutes(boolean showRoutes) {
        mShowRoutes = showRoutes;
    }
}
