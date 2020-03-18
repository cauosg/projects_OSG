using UnityEngine;
using System.Collections;

public class CameraController : MonoBehaviour
{
	public GameObject cameraTarget;
	public float rotateSpeed;
	float rotate;
	public float offsetDistance;
	public float offsetHeight;
	public float smoothing;
    public bool uprising = false;
    public float risingangle = 60.0f;
	Vector3 offset;
	bool following = true;
	Vector3 lastPosition;

	void Start()
	{
		cameraTarget = GameObject.FindGameObjectWithTag("Player");
		lastPosition = new Vector3(cameraTarget.transform.position.x, cameraTarget.transform.position.y + offsetHeight, cameraTarget.transform.position.z - offsetDistance);
		offset = new Vector3(cameraTarget.transform.position.x, cameraTarget.transform.position.y + offsetHeight, cameraTarget.transform.position.z - offsetDistance);
	}

	void Update()
	{
		if(Input.GetKeyDown(KeyCode.U))
		{
			if(following)
			{
				following = false;
			} 
			else
			{
				following = true;
			}
		} 
		if(Input.GetKey(KeyCode.I))
		{
			rotate = -1;
		} 
		else if(Input.GetKey(KeyCode.O))
		{
			rotate = 1;
		} 
		else
		{
			rotate = 0;
		}
		if(following)
		{
            
			offset = Quaternion.AngleAxis(rotate * rotateSpeed, Vector3.up) * offset;
            if (uprising)
                offset = Quaternion.AngleAxis(risingangle, Vector3.left) * offset;

            transform.position = cameraTarget.transform.position + offset; 
			transform.position = new Vector3(Mathf.Lerp(lastPosition.x, cameraTarget.transform.position.x + offset.x, smoothing * Time.deltaTime), 
				Mathf.Lerp(lastPosition.y, cameraTarget.transform.position.y + offset.y, smoothing * Time.deltaTime), 
				Mathf.Lerp(lastPosition.z, cameraTarget.transform.position.z + offset.z, smoothing * Time.deltaTime));
		} 
		else
		{
			transform.position = lastPosition; 
		}
		transform.LookAt(cameraTarget.transform.position);
	}

	void LateUpdate()
	{
        if (uprising)
            cameraTarget = GameObject.FindGameObjectWithTag("boss");
        else
            cameraTarget = GameObject.FindGameObjectWithTag("Player");
        lastPosition = transform.position;
	}
}