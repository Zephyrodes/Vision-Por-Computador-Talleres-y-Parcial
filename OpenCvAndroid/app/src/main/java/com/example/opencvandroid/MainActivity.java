package com.example.opencvandroid;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.JavaCamera2View;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Mat;

import java.io.InputStream;

public class MainActivity extends AppCompatActivity
        implements CameraBridgeViewBase.CvCameraViewListener2 {

    private static final String TAG = "MainActivity";

    static {
        System.loadLibrary("native-lib");
    }

    private JavaCamera2View cameraView;
    private ImageView imageView;
    private Button btnCamera, btnGallery;
    private Button btnOriginal, btnSuave, btnSketch, btnThermal;
    private ImageButton btnSwitchCamera;

    private int currentEffect = 0;
    private int mCameraId = CameraBridgeViewBase.CAMERA_ID_BACK;
    private Uri currentGalleryUri = null;

    public native void procesarFrame(long addrFrame, int effectId);

    private final ActivityResultLauncher<Intent> galleryLauncher = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> {
                if (result.getResultCode() == RESULT_OK && result.getData() != null) {
                    currentGalleryUri = result.getData().getData();
                    processGalleryImage(currentGalleryUri);
                }
            }
    );

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);

        if (!OpenCVLoader.initDebug()) {
            Log.e(TAG, "ERROR OpenCV");
        }

        cameraView = findViewById(R.id.camera_view);
        imageView = findViewById(R.id.image_view);
        btnCamera = findViewById(R.id.btn_camera);
        btnGallery = findViewById(R.id.btn_gallery);
        btnSwitchCamera = findViewById(R.id.btn_switch_camera);
        
        btnOriginal = findViewById(R.id.btn_original);
        btnSuave = findViewById(R.id.btn_sepia);
        btnSketch = findViewById(R.id.btn_sketch);
        btnThermal = findViewById(R.id.btn_thermal);

        cameraView.setCvCameraViewListener(this);
        cameraView.setCameraIndex(mCameraId);

        btnOriginal.setOnClickListener(v -> setEffect(0));
        btnSuave.setOnClickListener(v -> setEffect(1));
        btnSketch.setOnClickListener(v -> setEffect(2));
        btnThermal.setOnClickListener(v -> setEffect(3));

        btnCamera.setOnClickListener(v -> showCamera());
        btnGallery.setOnClickListener(v -> checkStoragePermissionAndOpenGallery());
        btnSwitchCamera.setOnClickListener(v -> switchCamera());

        showCamera();
    }

    private void switchCamera() {
        mCameraId = (mCameraId == CameraBridgeViewBase.CAMERA_ID_BACK) ? 
                     CameraBridgeViewBase.CAMERA_ID_FRONT : CameraBridgeViewBase.CAMERA_ID_BACK;
        
        cameraView.disableView();
        cameraView.setCameraIndex(mCameraId);
        cameraView.enableView();
    }

    private void setEffect(int effectId) {
        currentEffect = effectId;
        if (imageView.getVisibility() == View.VISIBLE && currentGalleryUri != null) {
            processGalleryImage(currentGalleryUri);
        }
    }

    private void showCamera() {
        imageView.setVisibility(View.GONE);
        btnSwitchCamera.setVisibility(View.VISIBLE);
        cameraView.setVisibility(View.VISIBLE);
        solicitarPermisoCamara();
    }

    private void checkStoragePermissionAndOpenGallery() {
        String permission = (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) ?
                Manifest.permission.READ_MEDIA_IMAGES : Manifest.permission.READ_EXTERNAL_STORAGE;

        if (ContextCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{permission}, 2);
        } else {
            openLocalGallery();
        }
    }

    private void openLocalGallery() {
        cameraView.disableView();
        cameraView.setVisibility(View.GONE);
        btnSwitchCamera.setVisibility(View.GONE);
        imageView.setVisibility(View.VISIBLE);

        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("image/*");
        intent.putExtra(Intent.EXTRA_LOCAL_ONLY, true);
        galleryLauncher.launch(Intent.createChooser(intent, "Selecciona una foto local"));
    }

    private void processGalleryImage(Uri uri) {
        try {
            InputStream inputStream = getContentResolver().openInputStream(uri);
            Bitmap bitmap = BitmapFactory.decodeStream(inputStream);

            Mat mat = new Mat();
            Utils.bitmapToMat(bitmap, mat);

            procesarFrame(mat.getNativeObjAddr(), currentEffect);

            Bitmap processedBitmap = Bitmap.createBitmap(mat.cols(), mat.rows(), Bitmap.Config.ARGB_8888);
            Utils.matToBitmap(mat, processedBitmap);

            imageView.setImageBitmap(processedBitmap);
            mat.release();
        } catch (Exception e) {
            Log.e(TAG, "Error: " + e.getMessage());
        }
    }

    private void solicitarPermisoCamara() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, 1);
        } else {
            enableCamera();
        }
    }

    private void enableCamera() {
        if (cameraView != null && cameraView.getVisibility() == View.VISIBLE) {
            cameraView.setCameraPermissionGranted();
            cameraView.enableView();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1 && grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            enableCamera();
        } else if (requestCode == 2 && grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            openLocalGallery();
        }
    }

    @Override
    public void onCameraViewStarted(int width, int height) {}

    @Override
    public void onCameraViewStopped() {}

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        Mat frame = inputFrame.rgba();
        procesarFrame(frame.getNativeObjAddr(), currentEffect);
        return frame;
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (cameraView != null) cameraView.disableView();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (cameraView != null && cameraView.getVisibility() == View.VISIBLE) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED) {
                enableCamera();
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (cameraView != null) cameraView.disableView();
    }
}
