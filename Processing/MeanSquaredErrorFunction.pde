float meanSquaredError(ArrayList<Float> data, ArrayList<Float> reference) {
    if (data.size() != reference.size() || data.isEmpty()) {
        return Float.NaN; // or throw IllegalArgumentException
    }

    float mse = 0.0f;
    for (int i = 0; i < data.size(); i++) {
        float diff = data.get(i) - reference.get(i);
        mse += diff * diff;
    }
    return mse / data.size();
}
