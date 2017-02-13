package com.example.vad.myapplication;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.net.Uri;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.ContextMenu;
import android.view.GestureDetector;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.view.ViewGroup;
import android.view.ViewManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.google.android.gms.appindexing.Action;
import com.google.android.gms.appindexing.AppIndex;
import com.google.android.gms.common.api.GoogleApiClient;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicInteger;

public class MainActivity extends AppCompatActivity {
    private ImageView mImageView;
    private TextView mTextView;
    private Button mButton;
    private Button mTspButton;
    private ArrayList<RHPoint> pointArray;
    private int curX;
    private int curY;
    private View mPoint;
    private ArrayList<SegmentView> segmentArray;
    private MainActivity context;
    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    private GoogleApiClient client;
    private DecimalFormat df;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        context = this;
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        curX = 0;
        curY = 0;
        pointArray = new ArrayList<RHPoint>();

        final GestureDetector gdt = new GestureDetector(new GestureListener());

        mImageView = (ImageView) findViewById(R.id.imageView);
        mImageView.setImageResource(R.drawable.mr2014_map);
        mTextView = (TextView) findViewById(R.id.textView);
        mTextView.setHeight(100);
        df = new DecimalFormat("#.##");
        mButton = (Button) findViewById(R.id.button);
        mButton.setEnabled(false);
        mTspButton = (Button) findViewById(R.id.tspButton);
        //mTspButton.setEnabled(false);

        registerForContextMenu(mImageView);

        //mImageView.setOnClickListener(viewClickListener);

        Button.OnClickListener deletePointListener = new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                for (int i = 0; i < pointArray.size(); ++i) {
                    if (mPoint == pointArray.get(i).getView()) {
                        pointArray.remove(i);
                        ((ViewManager) mPoint.getParent()).removeView(mPoint);
                        mButton.setEnabled(false);
                        break;
                    }
                }
            }
        };
        mButton.setOnClickListener(deletePointListener);

        View.OnTouchListener viewTouchListener = new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                gdt.onTouchEvent(event);

                float x = event.getX();
                float y = event.getY();
                String sDown = "down";
                String sMove = "move";
                String sUp = "up";

                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN: // нажатие
                        sDown = "Down: " + x + "," + y;
                        sMove = "";
                        sUp = "";
                        break;
                    case MotionEvent.ACTION_MOVE: // движение
                        sMove = "Move: " + x + "," + y;
                        break;
                    case MotionEvent.ACTION_UP: // отпускание
                    case MotionEvent.ACTION_CANCEL:
                        sMove = "";
                        sUp = "Up: " + x + "," + y;
                        break;
                }
                mTextView.setText(sDown + "\n" + sMove + "\n" + sUp);
                return false;
            }
        };

        mImageView.setOnTouchListener(viewTouchListener);


        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        Button.OnClickListener tspListener = new Button.OnClickListener() {
            @Override
            public void onClick(View v) {
                ArrayList<Double> matrix = generateDistanceMatrix();
                ArrayList<Integer> path = generatePath(matrix);

                for (int i = 0; i < pointArray.size(); ++i) {
                    int j = i + 1;
                    if (j == pointArray.size())
                        j = 0;
                    // draw segments
                    RHPoint p1 = pointArray.get(i);
                    RHPoint p2 = pointArray.get(j);
                    SegmentView v1 = new SegmentView(context, p1.getPoint().x, p1.getPoint().y,
                            p2.getPoint().x, p2.getPoint().y);

                    RelativeLayout rl2 = (RelativeLayout) mImageView.getParent();
                    RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(20, 20);

                    lp.setMargins(0, 0, 0, 0);
                    lp.leftMargin = mImageView.getLeft() - 30;
                    lp.topMargin = mImageView.getTop() - 30;
                    //v.setLayoutParams(lp);
                    rl2.addView(v, lp);
                    v1.setId(context.generateViewId());
                    segmentArray.add(v1);
                    String s = "Add point (" + curX + ", " + curY + ")\n";
                    mTextView.setText(s);
                }

                //registerForContextMenu(v);
//                View.OnClickListener pointClickListener = new View.OnClickListener() {
//                    @Override
//                    public void onClick(View v) {
//                        mButton.setEnabled(true);
//                        mTextView.setText("point clicked");
//                        mPoint = v;
//                    }
//                };
                //v.setOnClickListener(pointClickListener);
            }
        };
        mTspButton.setOnClickListener(tspListener);


        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client = new GoogleApiClient.Builder(this).addApi(AppIndex.API).build();
    }

    private ArrayList generateDistanceMatrix() {
        mTextView.setText(pointArray.size());
        ArrayList<Double> matrix = new ArrayList<Double>(pointArray.size()*pointArray.size());
        for (int i = 0; i < pointArray.size(); ++i) {
            for (int j = 0; j < pointArray.size(); ++j) {
                RHPoint p1 = pointArray.get(i);
                RHPoint p2 = pointArray.get(j);
                double distance = Math.sqrt((p1.getPoint().x-p2.getPoint().x)*(p1.getPoint().x-p2.getPoint().x) +
                        (p1.getPoint().y-p2.getPoint().y)*(p1.getPoint().y-p2.getPoint().y));
//                matrix.set(i*pointArray.size() + j, distance);
            }
        }

        return matrix;
    }

    private ArrayList generatePath(ArrayList<Double> matrix) { // nearest neighbour
        ArrayList<Integer> path = new ArrayList<Integer>(pointArray.size());
        double minValue = Double.MAX_VALUE;
        int mini = 0;
        int minj = 0;

//        for (int i = 0; i < pointArray.size(); ++i) {
//            for (int j = i + 1; j < pointArray.size(); ++j) {
//                double value = matrix.get(i*pointArray.size() + j);
//                if (value < minValue) {
//                    mini = i;
//                    minj = j;
//                }
//            }
//        }
        for (int i = 0; i < pointArray.size(); ++i) {
            path.set(i, i);
        }

        return path;
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onStart() {
        super.onStart();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client.connect();
        Action viewAction = Action.newAction(
                Action.TYPE_VIEW, // TODO: choose an action type.
                "Main Page", // TODO: Define a title for the content shown.
                // TODO: If you have web page content that matches this app activity's content,
                // make sure this auto-generated web page URL is correct.
                // Otherwise, set the URL to null.
                Uri.parse("http://host/path"),
                // TODO: Make sure this auto-generated app URL is correct.
                Uri.parse("android-app://com.example.vad.myapplication/http/host/path")
        );
        AppIndex.AppIndexApi.start(client, viewAction);
    }

    @Override
    public void onStop() {
        super.onStop();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        Action viewAction = Action.newAction(
                Action.TYPE_VIEW, // TODO: choose an action type.
                "Main Page", // TODO: Define a title for the content shown.
                // TODO: If you have web page content that matches this app activity's content,
                // make sure this auto-generated web page URL is correct.
                // Otherwise, set the URL to null.
                Uri.parse("http://host/path"),
                // TODO: Make sure this auto-generated app URL is correct.
                Uri.parse("android-app://com.example.vad.myapplication/http/host/path")
        );
        AppIndex.AppIndexApi.end(client, viewAction);
        client.disconnect();
    }

    public void onClick(View v){
        switch (v.getId()) {
            case R.id.imageView:
                //v.setImageResource(R.drawable.ic_action_search);
                //mImageView.getcli
                break;
            default:
                assert(false);
        }
    }

    private static final int SWIPE_MIN_DISTANCE = 120;
    private static final int SWIPE_THRESHOLD_VELOCITY = 200;

    private class GestureListener extends GestureDetector.SimpleOnGestureListener {
//        @Override
//        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
//            //mTextView.setText(e1.getAction());
//            if(e1.getX() - e2.getX() > SWIPE_MIN_DISTANCE && Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY) {
//                return false; // Right to left
//            }  else if (e2.getX() - e1.getX() > SWIPE_MIN_DISTANCE && Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY) {
//                return false; // Left to right
//            }
//
//            if(e1.getY() - e2.getY() > SWIPE_MIN_DISTANCE && Math.abs(velocityY) > SWIPE_THRESHOLD_VELOCITY) {
//                return false; // Bottom to top
//            }  else if (e2.getY() - e1.getY() > SWIPE_MIN_DISTANCE && Math.abs(velocityY) > SWIPE_THRESHOLD_VELOCITY) {
//                return false; // Top to bottom
//            }
//            return false;
//        }

        @Override
        public void onLongPress(MotionEvent e) {
            curX = (int)e.getX();
            curY = (int)e.getY();

//            String s = "Long press " + e.getX() + ", " + e.getY() + "\n";
//            mTextView.setText(s);
        }
    }

    final int MENU_POINT_ADD      = 1;
    final int MENU_POINT_REMOVE   = 2;
    final int MENU_COLOR_RED      = 3;
    final int MENU_COLOR_GREEN    = 4;
    final int MENU_COLOR_BLUE     = 5;

    @Override
    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenu.ContextMenuInfo menuInfo) {
        switch (v.getId()) {
            case R.id.imageView:
                menu.add(0, MENU_POINT_ADD, 0, "Добавить точку");
                menu.add(0, MENU_COLOR_RED, 0, "Red");
                menu.add(0, MENU_COLOR_GREEN, 0, "Green");
                menu.add(0, MENU_COLOR_BLUE, 0, "Blue");
                break;
        }
        for (int i = 0; i < pointArray.size(); ++i) {
            if (v.getId() == pointArray.get(i).getView().getId()) {
                menu.add(0, MENU_POINT_REMOVE, 0, "Удалить точку " + v.getId());
            }
        }
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {
        //AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo) item.getMenuInfo();
        //int index = info.position;

        switch (item.getItemId()) {
            case MENU_POINT_ADD:
                DrawView v = new DrawView(this);

                RelativeLayout rl2 = (RelativeLayout) mImageView.getParent();
                RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(20, 20);

                lp.setMargins(0, 0, 0, 0);
                lp.leftMargin = mImageView.getLeft() - 30 + curX;
                lp.topMargin = mImageView.getTop() - 30 + curY;
                //v.setLayoutParams(lp);
                rl2.addView(v, lp);
                v.setId(this.generateViewId());

                RHPoint p = new RHPoint(curX, curY, v);
                pointArray.add(p);
                String s = "Add point (" + curX + ", " + curY +  ")\n";
                mTextView.setText(s);

                //registerForContextMenu(v);
                View.OnClickListener pointClickListener = new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        mButton.setEnabled(true);
                        mTextView.setText("point clicked");
                        mPoint = v;
                    }
                };
                v.setOnClickListener(pointClickListener);
                break;

            case MENU_POINT_REMOVE:
                mTextView.setTextColor(Color.RED);
                mTextView.setText("Надо бы удалить точку");
                break;

            case MENU_COLOR_RED:
                mTextView.setTextColor(Color.RED);
                mTextView.setText("Text color = red");
                break;
            case MENU_COLOR_GREEN:
                mTextView.setTextColor(Color.GREEN);
                mTextView.setText("Text color = green");
                break;
            case MENU_COLOR_BLUE:
                mTextView.setTextColor(Color.BLUE);
                mTextView.setText("Text color = blue");
                break;
        }
        return super.onContextItemSelected(item);
    }

    class DrawView extends View {

        public DrawView(Context context) {
            super(context);
        }

        @Override
        protected void onDraw(Canvas canvas) {
            //canvas.drawColor(Color.GREEN);
            Paint p = new Paint();
            p.setColor(Color.RED);
            canvas.drawCircle(10, 10, 10, p);
            //canvas.draw
        }

    }

    class SegmentView extends View {

        private float x1;
        private float x2;
        private float y1;
        private float y2;


        public SegmentView(Context context, float x1, float y1, float x2, float y2) {
            super(context);
            this.x1 = x1;
            this.y1 = y1;
            this.x2 = x2;
            this.y2 = y2;
        }

        @Override
        protected void onDraw(Canvas canvas) {
            //canvas.drawColor(Color.GREEN);
            Paint p = new Paint();
            p.setColor(Color.RED);
            canvas.drawLine(x1, y1, x2, y2, p);
            //canvas.drawCircle(10, 10, 10, p);
            //canvas.draw
        }

    }

    private static final AtomicInteger sNextGeneratedId = new AtomicInteger(1);

    /**
     * Generate a value suitable for use in {@link #setId(int)}.
     * This value will not collide with ID values generated at build time by aapt for R.id.
     *
     * @return a generated ID value
     */
    public static int generateViewId() {
        for (;;) {
            final int result = sNextGeneratedId.get();
            // aapt-generated IDs have the high byte nonzero; clamp to the range under that.
            int newValue = result + 1;
            if (newValue > 0x00FFFFFF) newValue = 1; // Roll over to 1, not 0.
            if (sNextGeneratedId.compareAndSet(result, newValue)) {
                return result;
            }
        }
    }
}
