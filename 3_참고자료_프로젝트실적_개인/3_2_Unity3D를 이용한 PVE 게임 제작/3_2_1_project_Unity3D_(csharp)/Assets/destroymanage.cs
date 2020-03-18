using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class destroymanage : MonoBehaviour {

    public GameObject handrock;
	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
        if (handrock.transform.position.y < -100)
            Destroy(gameObject);
	}
}
