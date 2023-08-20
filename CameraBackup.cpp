////������ ��������� �����
//int boardWidth = 7;
//int boardHeight = 7;
//float squareSize = 0.02f; //  2�� ������� �������� ��������� �����
//
////������, ���������� ���� ��������� �����, ��������� �� ������ �����������
//std::vector<std::vector<cv::Point2f>> imagePoints;
//
////������ �����������
//std::vector<cv::Mat> images;
//for (const auto& entry : std::filesystem::directory_iterator("C:/CameraCalibrationPhotos")) {
//	if (entry.path().extension() == ".jpg") { // �������������, ��� ��� JPG ����
//		cv::Mat img = cv::imread(entry.path().string());
//		if (img.empty()) {
//			std::cout << "Cannot load the image: " << entry.path() << std::endl;
//			continue;
//		}
//		images.push_back(img);
//	}
//}
//
////������ ��� ����� ��������� �����
//std::vector<cv::Point2f> chessboard�orners;
////����� ����� �� �����������
//for (const auto& img : images) {
//	//������� ��� ������ ����� ��������� �����
//	bool found = cv::findChessboardCorners(img, cv::Size(boardWidth, boardHeight), chessboard�orners);
//	if (found) {
//		imagePoints.push_back(chessboard�orners);
//	}
//}
//
////�������� 3� ��������� ����� ��������� �����
//std::vector<std::vector<cv::Point3f>> objectPoints(1);
//for (int i = 0; i < boardHeight; i++) {
//	for (int j = 0; j < boardWidth; j++) {
//		objectPoints[0].push_back(cv::Point3f(j * squareSize, i * squareSize, 0));
//	}
//}
//objectPoints.resize(imagePoints.size(), objectPoints[0]);
//
//
//std::vector<cv::Mat> calibRvecs, calibTvecs;
////���������� ������ �� ������ ��������� ����� �� ����������� � �� 3� �����������
//cv::calibrateCamera(objectPoints, imagePoints, images[0].size(), cameraMatrix, distCoeffs, calibRvecs, calibTvecs);
//
//std::cout << "Camera Matrix: " << cameraMatrix << std::endl;
//std::cout << "Distortion coefficients: " << distCoeffs << std::endl;
//
////���������� ���������
//cv::Mat undistorted;
//cv::undistort(images[0], undistorted, cameraMatrix, distCoeffs);
//cv::imshow("Undistorted image", undistorted);
//cv::waitKey(0);
//
////���������� ���������� ������
//cv::FileStorage fs("camera_parameters.yml", cv::FileStorage::WRITE);
//fs << "cameraMatrix" << cameraMatrix;
//fs << "distCoeffs" << distCoeffs;
//fs.release();

	//try
	//{
	//	//rvecs.clear();
	//	//tvecs.clear();
	//	cv::aruco::detectMarkers(frame, dictionary, corners, ids);
	//	if (ids.size() > 0) {
	//		cv::aruco::estimatePoseSingleMarkers(corners, squareSize, cameraMatrix, distCoeffs, rvecs, tvecs);

	//		for (int i = 0; i < ids.size(); i++) {
	//			if (corners.empty()) {
	//				std::cerr << "No corners detected" << std::endl;
	//			}
	//			if (i >= rvecs.size() || i >= tvecs.size()) {
	//				std::cerr << "Error: rvecs or tvecs do not have enough elements" << std::endl;
	//			}
	//			cv::Vec3d rvec = rvecs[i];
	//			cv::Vec3d tvec = tvecs[i];
	//			drawAxis(frame, cameraMatrix, distCoeffs, rvec, tvec, 0.05);
	//		}
	//	}

	//}
	//catch (const std::exception& e)
	//{
	//	std::cerr << "openCV exception: " << e.what() << std::endl;
	//}


	//try
	//{
	//	cv::aruco::detectMarkers(frame, dictionary, corners, ids);
	//	if (ids.size() > 0) {
	//		for (int i = 0; i < ids.size(); i++) {
	//			// ��������� ���������� ��� ���������� ������ (��������, ����� ������� ���� �������)
	//			cv::Point2f textPosition = corners[i][0];
	//			// ����������� ID ������� � ������
	//			std::string idStr = std::to_string(ids[i]);
	//			// ������ ID �� �����������
	//			cv::putText(frame, idStr, textPosition, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);
	//		}
	//	}
	//	//cv::imshow("Detected ArUco markers", frame);
	//	cv::waitKey(0);
	//}
	//catch (const std::exception& e)
	//{
	//	std::cerr << "openCV exception: " << e.what() << std::endl;
	//}

// 2 ����

		//glfwMakeContextCurrent(window2);
		//shaderProgram2.Activate();

		//ShaderHelper::PassMatrix(matrix, locationMatrix);
		//ShaderHelper::PassVariable(color, locationVariable);

		//VAO1.Bind();
		//VBO1.Bind();
		//VAO1.BindEBO(EBO1);

		//VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
		//VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		//glBindTexture(GL_TEXTURE_2D, texture2);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glfwGetFramebufferSize(window2, &width, &height);
		//glViewport(0, 0, width, height);
		//glClear(GL_COLOR_BUFFER_BIT);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glfwSwapBuffers(window2);
		//VAO1.Unbind();
		//VBO1.Unbind();
		//VAO1.UnbindEBO(EBO1);

		//glfwPollEvents();


					//�������� ������� ���� �� ������ �������� �������� � ����������
					//glm::mat4 markerModel = convertRodriguesToMat4(rvecs[i], tvecs[i]);
					//markerModel = glm::scale(markerModel, glm::vec3(0.5f, 0.5f, 0.5f));
					//markerModel = glm::rotate(markerModel, 0.05f, glm::vec3(0.0f, 1.0f, 0.0f));
					//markerModel = glm::translate(markerModel, cube2Pos);
					//ShaderHelper::PassMatrix(glm::value_ptr(markerModel), locationModelCube);

					//VAO2.Bind();
					//modelCube = glm::rotate(modelCube, 0.05f, glm::vec3(0.0f, 1.0f, 0.0f));
					//ShaderHelper::PassMatrix(glm::value_ptr(modelCube), locationModelCube);
					//glBindTexture(GL_TEXTURE_2D, textureCube);
					//glDrawArrays(GL_TRIANGLES, 0, 36);
					//VAO2.Unbind();