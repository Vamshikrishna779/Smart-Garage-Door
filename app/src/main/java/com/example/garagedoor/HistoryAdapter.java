package com.example.garagedoor;import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import androidx.recyclerview.widget.RecyclerView;

import java.util.List;

public class HistoryAdapter extends RecyclerView.Adapter<HistoryAdapter.HistoryViewHolder> {
    private List<HistoryItem> historyList;

    public HistoryAdapter(List<HistoryItem> historyList) {
        this.historyList = historyList;
    }

    @Override
    public HistoryViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(parent.getContext()).inflate(R.layout.history_item, parent, false);
        return new HistoryViewHolder(itemView);
    }

    @Override
    public void onBindViewHolder(HistoryViewHolder holder, int position) {
        HistoryItem item = historyList.get(position);
        holder.statusTextView.setText(item.getStatus());
        holder.timestampTextView.setText(item.getTimestamp());
    }

    @Override
    public int getItemCount() {
        return historyList.size();
    }

    public static class HistoryViewHolder extends RecyclerView.ViewHolder {
        public TextView statusTextView;
        public TextView timestampTextView;

        public HistoryViewHolder(View itemView) {
            super(itemView);
            statusTextView = itemView.findViewById(R.id.statusTextView);
            timestampTextView = itemView.findViewById(R.id.timestampTextView);
        }
    }
}
