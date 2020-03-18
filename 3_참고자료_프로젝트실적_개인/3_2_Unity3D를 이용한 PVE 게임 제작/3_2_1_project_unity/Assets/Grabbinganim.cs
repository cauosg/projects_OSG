using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Grabbinganim : MonoBehaviour {
    public float animheight = 0.0f;
    Animator anim;
	// Use this for initialization
	void Start () {
		anim = GetComponent<Animator>();
    }
	
	// Update is called once per frame
	void Update () {
        if (transform.position.y > animheight)
            anim.Play("handrockgrab1");
	}
}
