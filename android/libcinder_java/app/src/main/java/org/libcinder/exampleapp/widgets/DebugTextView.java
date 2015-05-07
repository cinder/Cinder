package org.libcinder.exampleapp.widgets;

import android.annotation.TargetApi;
import android.content.Context;
import android.util.AttributeSet;
import android.widget.TextView;

import java.text.SimpleDateFormat;
import java.util.Calendar;

public class DebugTextView extends TextView {
    public DebugTextView(Context context) {
        super(context);
    }

    public DebugTextView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public DebugTextView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @TargetApi(21)
    public DebugTextView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }

    public void appendLine(String s, int indentSteps) {
        String indent = "";
        for(int i = 0; i < indentSteps; ++i) {
            indent += "  ";
        }

        Calendar cal = Calendar.getInstance();
        String time = (new SimpleDateFormat("HH:mm:ss")).format(cal.getTime());
        this.append( "[" + time + "]: " + indent + s + "\n");
    }

    public void appendLine(String s) {
        appendLine(s, 0);
    }
}
