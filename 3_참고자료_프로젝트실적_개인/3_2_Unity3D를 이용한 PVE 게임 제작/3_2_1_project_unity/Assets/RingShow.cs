using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RingShow : MonoBehaviour {

    public float showtime = 0.2f;
    private float showcount = 0.0f;
	// Use this for initialization
	void Start () {
        transform.position = new Vector3(transform.position.x, 0, transform.position.z-2.0f);
	}
	
	// Update is called once per frame
	void Update () {
        showcount += Time.deltaTime;
        if (showcount > showtime)
            transform.position = new Vector3(transform.position.x, -50, transform.position.z);
    }
}
